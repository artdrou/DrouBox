# ARM Cortex-M7 toolchain file for Daisy Seed (STM32H750)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Find toolchain — prefer arm-none-eabi-gcc in PATH
find_program(ARM_GCC arm-none-eabi-gcc REQUIRED)
get_filename_component(TOOLCHAIN_BIN_DIR "${ARM_GCC}" DIRECTORY)

set(CMAKE_C_COMPILER   "${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_BIN_DIR}/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${TOOLCHAIN_BIN_DIR}/arm-none-eabi-gcc")
set(CMAKE_OBJCOPY      "${TOOLCHAIN_BIN_DIR}/arm-none-eabi-objcopy")
set(CMAKE_SIZE         "${TOOLCHAIN_BIN_DIR}/arm-none-eabi-size")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Cortex-M7 with FPU (STM32H750 = Cortex-M7 + FPU double precision)
set(CPU_FLAGS "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb")

set(CMAKE_C_FLAGS_INIT   "${CPU_FLAGS} -fdata-sections -ffunction-sections")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS} -fdata-sections -ffunction-sections -fno-exceptions -fno-rtti")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS_INIT
    "${CPU_FLAGS} -Wl,--gc-sections -Wl,-Map=output.map,--cref -specs=nosys.specs -specs=nano.specs")

# Linker script is provided per-firmware target via daisy_add_linker_script() or manually
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
