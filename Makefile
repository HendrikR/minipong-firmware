ARDUINO_DIR = /usr/share/arduino
ALTERNATE_CORE = sparkfun
ARDUINO_SKETCHBOOK = .

# Important: determine the variant of ProMicro.
# can be ARDUINO_AVR_PROMICRO or ARDUINO_AVR_PRO .
CPPFLAGS = -DARDUINO_AVR_PROMICRO
MCU=atmega32u4



# Upload params
#
## ISP upload
ISP_PROG = stk500v2
ISP_PORT = /dev/ttyUSB0
AVRDUDE_ISP_BAUDRATE=115200
#
#
## ARDUINO NANO (Christians Technik-Shop, mit Mini-USB und 6Pin-ISP)
#ARDUINO_PORT = /dev/ttyUSB1
#BOARD_TAG=nano
#VARIANT=eightanaloginputs
#ALTERNATE_CORE=
#AVRDUDE_ARD_BAUDRATE = 115200 # Nano only wants to work with this baud rate.
#
#
## ARDUINO Micro (Billig-Kopie von Amazon/Christians Technik-Shop, ohne Micro-USB)
#ARDUINO_PORT = /dev/ttyUSB1
#AVRDUDE_ARD_PROGRAMMER = stk500v2
#AVRDUDE_ARD_BAUDRATE = 115200 # default for the ProMicro is 57600, but my 3.3V board refuses to work with that
#MONITOR_PORT = /dev/ttyUSB0
#ISP_PROG = stk500v2
#ISP_PORT = /dev/ttyUSB0
#AVRDUDE_MCU = m328p
#AVRDUDE_OPTS = -v -e

## ARDUINO Micro ("Original" von Christians Technik-Shop, mit Micro-USB)
ARDUINO_PORT = /dev/ttyACM0
AVRDUDE_ARD_PROGRAMMER = avr109
##AVRDUDE_ARD_BAUDRATE = 115200
BOARD_TAG = promicro # used for build dir
BOARD_SUB = 8MHzatmega32U4
VARIANT = promicro # used for include dir
RESET_CMD = ./bootmode.py $(ARDUINO_PORT)

#AVRDUDE_OPTS = -v -e

#TARGET       = minipong

include ../../_MAKEFILE/Arduino.mk

# Verbindungen
# ARDUINO PRO Mini (3.3V-Variante):
# MOSI: Arduino Digital 11 (ISP in)
# MISO: Arduino Digital 12 (ISP out)
# SCK: Arduino Digital 13 (ISP Sync Clock)
#
# Arduino Pro Micro (5V-Variante)
# MOSI: 16
# MISO: 14
# SCK:  15
# Die Ports heißen zwar anders als beim Pro Mini, sind aber an der gleichen Position.
# mySmartUSB light-Stecker:
# (männl., mit Sicht auf die Pinholes, "rot" ist die Seite mit dem roten Kabel in Richtung des USB-Sticks, die Striche oben(_-_) markieren die Orientierungsnut)
#     | _____|------|____
#     | MISO | SCK  | RST
# rot | VTG  | MOSI | GND
# Anschlüsse
# USB  <--> Pro Mini
# VTG  <--> RAW oder VCC
# GND  <--> GND
# MOSI <--> D11 (MOSI)
# MISO <--> D12 (MISO)
# SCK  <--> D13 (SCK)
# RST  <--> RST
# ___ACHTUNG___: Manchmal funktionierte beim Mini das RST auf der Seite mit der analogen Pins nicht, aber das auf der Gegenseite (neben den TXI/RXO-Pins).
#
# MANUELLE VARIANTE: avrdude -p m328p -c stk500v2 -P /dev/ttyUSB0 -U flash:w:build-promicro/minipong-promicro.hex -b 115200
#

# Per "Arduino Uno as ISP"
# TODO: Hat scheinbar funktioniert, ich habe aber den Verdacht, es wurde nur auf den Uno geschrieben.
# Dazu muss zunächst die passende Skizze aus /usr/share/arduino/examples/11.ArduinoISP auf den Uno geladen werden.
# Anleitung hier: https://ouilogique.com/arduino-pro-mini/#prog1
# Verbindungen so:
# UNO <--> Pro Mini
# 5V  <--> RAW ⚠
# GND <--> GND
# 10  <--> RST
# 11  <--> 11
# 12  <--> 12
# 13  <--> 13
# Dann: 
#avrdude -p atmega328p -c stk500 -P /dev/ttyACM0 -U flash:w:build-promicro/minipong-promicro.hex
