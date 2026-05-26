cmake_minimum_required(VERSION 3.22)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
)
FetchContent_MakeAvailable(Catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

# Helper: register a test executable
function(add_droubox_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})
    target_link_libraries(${TEST_NAME} PRIVATE droubox::dsp Catch2::Catch2WithMain)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)
    catch_discover_tests(${TEST_NAME} DISCOVERY_MODE PRE_TEST)
endfunction()

# Helper: register a controls test (links against stub-backed controls lib)
function(add_droubox_controls_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})
    target_link_libraries(${TEST_NAME} PRIVATE droubox::controls Catch2::Catch2WithMain)
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stubs)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)
    catch_discover_tests(${TEST_NAME} DISCOVERY_MODE PRE_TEST)
endfunction()

add_droubox_test(test_lfo         dsp/test_lfo.cpp)
add_droubox_test(test_shelf       dsp/test_shelf.cpp)
add_droubox_test(test_mapping     dsp/test_mapping.cpp)
add_droubox_test(test_compressor  dsp/test_compressor.cpp)
add_droubox_test(test_distortion  dsp/test_distortion.cpp)
add_droubox_test(test_yin         dsp/test_yin.cpp)

add_droubox_controls_test(test_footswitch      controls/test_footswitch.cpp)
add_droubox_controls_test(test_footswitch_pair controls/test_footswitch_pair.cpp)
