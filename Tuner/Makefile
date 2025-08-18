# Project Name
TARGET = Tuner

# Sources
CPP_SOURCES = main.cpp controls.cpp \
../../../drouAudioLib/src/mapping.cpp \
../../../drouAudioLib/src/fft.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

C_INCLUDES += -I../../../drouAudioLib/include