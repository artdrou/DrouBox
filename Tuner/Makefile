# Project Name
TARGET = Tuner

# Automatically collect all cpp sources in src and subfolders
CPP_SOURCES := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp) $(wildcard *.cpp)

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR  = ../../DaisySP/

# Core location
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core

# Include paths
C_INCLUDES += -I./include \
              -I./include/controls \
			  -I../../../drouAudioLib \
              -I../../../drouAudioLib/include

# Include libDaisy build system
include $(SYSTEM_FILES_DIR)/Makefile
