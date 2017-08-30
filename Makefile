## Compiler
CC=gdcc-cc
LD=gdcc-ld
MAKELIB=gdcc-makelib

## Compiler flags
TARGET=--bc-target=ZDoom
LFLAGS=$(TARGET) --bc-zdacs-init-delay --bc-zdacs-chunk-STRE
CFLAGS=$(TARGET)

## Sources
IR=ir
SRCDIR=source

PK7_BIN=pk7/acs
PK7_SRCDIR=pk7/lheader

LIB_OUTPUTS=$(IR)/libc.ir $(IR)/libGDCC.ir
LIB_BINARY=$(PK7_BIN)/lithlib.bin

MAIN_IR=$(IR)/main
MAIN_SRC=$(SRCDIR)/Main
MAIN_INC=$(PK7_SRCDIR)
MAIN_SOURCES=$(wildcard $(MAIN_SRC)/*.c)
MAIN_HEADERS=$(wildcard $(MAIN_INC)/*.h)
MAIN_OUTPUTS=$(MAIN_SOURCES:$(MAIN_SRC)/%.c=$(MAIN_IR)/%.ir)
MAIN_BINARY=$(PK7_BIN)/lithmain.bin
MAIN_CFLAGS=-i$(MAIN_INC) -Dnull=NULL --sys-include $(LITHOS_INC)

LITHOS_IR=$(IR)/lithos
LITHOS_SRC=$(SRCDIR)/LithOS3/lithos_c/src
LITHOS_INC=$(SRCDIR)/LithOS3/lithos_c/inc
LITHOS_SOURCES=$(wildcard $(LITHOS_SRC)/*.c)
LITHOS_HEADERS=$(wildcard $(LITHOS_INC)/*.h)
LITHOS_OUTPUTS=$(LITHOS_SOURCES:$(LITHOS_SRC)/%.c=$(LITHOS_IR)/%.ir)
LITHOS_CFLAGS=-i$(LITHOS_INC)

## Targets
.PHONY: bin text clean

all: text bin
bin: $(PK7_BIN)/lithlib.bin $(PK7_BIN)/lithmain.bin

text: compilefs.rb
	@cd filedata; ../compilefs.rb Directory.txt

clean:
	@rm -f $(MAIN_OUTPUTS) $(LITHOS_OUTPUTS) $(LIB_OUTPUTS)

## .ir -> .bin
$(LIB_BINARY): $(LIB_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) --alloc-min Sta "" 300000000 $^ -o $@

$(MAIN_BINARY): $(MAIN_OUTPUTS) $(LITHOS_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) --alloc-min Sta "" 3000000000 $^ -o $@ -llithlib

## .c -> .ir
$(MAIN_IR)/%.ir: $(MAIN_SRC)/%.c $(MAIN_HEADERS) $(LITHOS_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(MAIN_CFLAGS) -c $< -o $@

$(LITHOS_IR)/%.ir: $(LITHOS_SRC)/%.c $(LITHOS_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(LITHOS_CFLAGS) -c $< -o $@

$(IR)/libc.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libc -o $@

$(IR)/libGDCC.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libGDCC -o $@

## EOF
