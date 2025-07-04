# Project Name
TARGET = Delay

# Sources
CPP_SOURCES = main.cpp \
disto.cpp \
utils.cpp \
controls.cpp \
filters.cpp \
compressor.cpp \
delay.cpp \
../../../drouAudioLib/src/lfo.cpp

# Library Locations
LIBDAISY_DIR = ../../libDaisy/
DAISYSP_DIR = ../../DaisySP/

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

C_INCLUDES += -I../../../drouAudioLib/include
