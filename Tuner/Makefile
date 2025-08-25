# Project Name
TARGET = Tuner

# Automatically collect all cpp sources in src and subfolders
CPP_SOURCES :=  $(wildcard src/**/**/*.cpp) \
                $(wildcard src/**/*.cpp) \
                $(wildcard src/*.cpp) \
                $(wildcard *.cpp) \
                $(wildcard ../lib/src/**/*.cpp) \
                $(wildcard ../lib/src/*.cpp) \
                $(wildcard ../lib/*.cpp)

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR  = ../../DaisySP/

# Core location
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

# Include paths
C_INCLUDES += -I./include \
              -I./include/controls \
              -I./include/app \
              -I./include/app/core \
              -I./include/app/effects \
              -I ../lib/include \
              -I ../lib/include/controls \
              -I../../../drouAudioLib \
              -I../../../drouAudioLib/include
              

# Include libDaisy build system
include $(SYSTEM_FILES_DIR)/Makefile
