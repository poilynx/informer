CC = gcc
RM = rm -f
AR = ar
AS = nasm
LINK = ld

INC = $(TOP)/include
BIN = $(TOP)/bin
LIB = $(TOP)/lib

ifeq ($(PROJ), shellcode)
	CFLAGS = -I$(INC) -Wall  -Os -m32
	ASFLAGS = -felf -Ox
else
	CFLAGS = -I$(INC) -Wall -g
	LDFLAGS = -e _start
endif
