# Name		: Makefile
# Project	: test Button library
# Author	: Bernd Waldmann
# Created	: 16-Aug-2021
# Tabsize	: 4
#
# This Revision: $Id: Makefile 1237 2021-08-17 10:17:55Z  $

## ----- General Flags

PROJECT = test_Button
MCU = atmega328p
F_CPU = 8000000
ROMSIZE = 30000
RAMSIZE = 2048

## ----- source files
CPPSOURCES = main.cpp Button.cpp AvrTimerBase.cpp AvrTimer2.cpp
CSOURCES = 
ASOURCES = 

SHAREDPATH = ../../../
include ../../../mk.d/bw-avr-defines.mk
#COMMON_Cxx += -Wa,-adhln=$(*F).s -g -fverbose-asm
include ../../../mk.d/fuses-ATmega328p-intRC.mk
include ../../../mk.d/bw-avr-rules.mk
