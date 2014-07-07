
ARDUINO_LIBS = SPI
ARDUINO_PORT = /dev/ttyACM0

BOARD_TAG = leonardo
#BOARD_TAG = uno
#BOARD_TAG = minimus32

ifeq ($(BOARD_TAG),uno)
#BOARDS_TXT=$(ARDUINO_SKETCHBOOK)/hardware/formulad/boards.txt
ARDUINO_PORT = usb
AVRDUDE_ARD_PROGRAMMER=stk500v2
RESET_CMD = /bin/true
endif

ifeq ($(BOARD_TAG),minimus32)
BOARDS_TXT=$(ARDUINO_SKETCHBOOK)/hardware/minimus/boards.txt
ARDUINO_VAR_PATH=$(ARDUINO_SKETCHBOOK)/hardware/minimus/variants
ARDUINO_CORE_PATH=$(ARDUINO_SKETCHBOOK)/hardware/minimus/cores/minimus
endif

include /home/paul/bin/Arduino.mk
