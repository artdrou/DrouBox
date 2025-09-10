# Project Name
TARGET = MultiEffect

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
              -I./include/utils \
              -I ../lib/include \
              -I ../lib/include/controls \
              -I ../lib/include/effects \
              -I ../lib/include/dsp \
              -I ../lib/include/utils \
              -I ../lib/include/core
              

# Include libDaisy build system
include $(SYSTEM_FILES_DIR)/Makefile
