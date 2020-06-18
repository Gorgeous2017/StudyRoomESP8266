# Main settings includes
#include	../settings.mk
# build directory
BUILD_BASE	= .build

# firmware directory
FW_BASE		= bin

# name for the target project
TARGET		= esp8266_iot

# Base directory for the compiler
SDK_BASE	?= ../ESP8266_NONOS_SDK
SDK_TOOLS	?= $(SDK_BASE)/tools/

# Extra libs, include and ld file
EXTRA_BASE	?= $(SDK_BASE)/include

BOOT ?= none
APP ?= 0
SPI_SPEED ?= 40
SPI_MODE ?= DIO
SPI_SIZE_MAP ?= 6

#VERBOSE = 1

# Basic project settings
MODULES	= user
LIBS	= c gcc hal phy pp net80211 lwip wpa crypto main json 
# Root includes
include	../common_nonos.mk
