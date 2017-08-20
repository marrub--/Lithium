## Compiler

CC=gdcc-cc
LD=gdcc-ld
MAKELIB=gdcc-makelib

## Directories

BIN=pk7/acs
SRCDIR=pk7/lcode
SRC=$(SRCDIR)/Main
INC=$(SRC)
IR=ir

LITHOS_SRC=$(SRCDIR)/LithOS3/lithos_c/src
LITHOS_INC=$(SRCDIR)/LithOS3/lithos_c/inc

## Compiler flags

TARGET=--bc-target=ZDoom
LFLAGS=$(TARGET) --bc-zdacs-init-delay --bc-zdacs-chunk-STRE
CFLAGS=$(TARGET) -i$(LITHOS_INC) --sys-include $(LITHOS_INC)
ALLOCMIN=--alloc-min Sta ""

## Sources

MAIN_SOURCES=$(wildcard $(SRC)/*.c)
MAIN_HEADERS=$(wildcard $(INC)/*.h)
MAIN_OUTPUTS=$(MAIN_SOURCES:$(SRC)/%.c=$(IR)/%.ir)

LITHOS_SOURCES=$(wildcard $(LITHOS_SRC)/*.c)
LITHOS_HEADERS=$(wildcard $(LITHOS_INC)/*.h)
LITHOS_OUTPUTS=$(LITHOS_SOURCES:$(LITHOS_SRC)/%.c=$(IR)/lithos_%.ir)

## all

.PHONY: bin text

all: text bin
bin: $(BIN)/lithlib.bin $(BIN)/lithmain.bin
text: $(SRCDIR)/FileData/compilefs.lua
	@cd $(SRCDIR)/FileData; lua compilefs.lua

## acs/*.bin

$(BIN)/lithlib.bin: $(IR)/libc.ir $(IR)/libGDCC.ir
	@echo LD $@
	@$(LD) $(LFLAGS) $(ALLOCMIN) 300000000  $^ -o $@

$(BIN)/lithmain.bin: $(MAIN_OUTPUTS) $(LITHOS_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) $(ALLOCMIN) 3000000000 $^ -o $@ -llithlib

## ir/*.ir

$(IR)/%.ir: $(SRC)/%.c $(MAIN_HEADERS) $(LITHOS_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) -i$(INC) -Dnull=NULL -c $< -o $@

$(IR)/lithos_%.ir: $(LITHOS_SRC)/%.c $(LITHOS_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(IR)/libc.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libc -o $@

$(IR)/libGDCC.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libGDCC -o $@

## EOF
