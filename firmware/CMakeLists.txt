cmake_minimum_required(VERSION 3.22)

# Helper macro: create a firmware target linked to libDaisy + droubox
macro(add_firmware TARGET_NAME)
    # Remaining args are source files
    set(SOURCES ${ARGN})

    add_executable(${TARGET_NAME} ${SOURCES})

    target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/lib/include
        ${LIBDAISY_DIR}/src
        ${LIBDAISY_DIR}/src/sys
        ${LIBDAISY_DIR}/Drivers/CMSIS/Include
        ${LIBDAISY_DIR}/Drivers/CMSIS/Device/ST/STM32H7xx/Include
        ${DAISYSP_DIR}/Source
    )

    target_link_libraries(${TARGET_NAME} PRIVATE droubox daisy DaisySP)

    # Generate .bin for DFU flashing
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${TARGET_NAME}>
            ${CMAKE_BINARY_DIR}/${TARGET_NAME}.bin
        COMMENT "Creating ${TARGET_NAME}.bin"
    )
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_NAME}>
        COMMENT "Firmware size:"
    )
endmacro()

add_subdirectory(delay)
add_subdirectory(distortion)
add_subdirectory(looper)
add_subdirectory(tuner)
add_subdirectory(multieffect)
