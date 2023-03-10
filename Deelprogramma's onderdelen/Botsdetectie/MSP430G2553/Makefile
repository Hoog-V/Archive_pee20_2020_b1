OBJECTS=main.o
MAP=main.map
MAKEFILE=Makefile

ifeq ($(OS),Windows_NT)
	ifeq ($(shell uname -o),Cygwin)
		RM= rm -rf
	else
		RM= del /q
	endif
else
	RM= rm -rf
endif

GCC_DIR = /opt/msp430-gcc/bin
SUPPORT_FILE_DIRECTORY = /opt/msp430-gcc/include

DEVICE  = MSP430G2553
CC      = $(GCC_DIR)/msp430-elf-gcc
GDB     = $(GCC_DIR)/msp430-elf-gdb

CFLAGS = -I $(SUPPORT_FILE_DIRECTORY) -mmcu=$(DEVICE) -Og -Wall -g
LFLAGS = -L $(SUPPORT_FILE_DIRECTORY) -Wl,-Map,$(MAP),--gc-sections 

all: ${OBJECTS}
	$(CC) $(CFLAGS) $(LFLAGS) $? -o $(DEVICE).out

clean: 
	$(RM) $(OBJECTS)
	$(RM) $(MAP)
	$(RM) *.out
	
debug: all
	$(GDB) -eval-command="target remote :55000" --eval-command=load --eval-command=c $(DEVICE).out
