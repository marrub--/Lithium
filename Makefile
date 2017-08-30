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

LIB_OUTPUTS=$(IR)/libc.ir $(IR)/libGDCC.ir
LIB_BINARY=$(PK7_BIN)/lithlib.bin

MAIN_IR=$(IR)/main
MAIN_SRC=$(SRCDIR)/Main
MAIN_INC=pk7/lheader
MAIN_SOURCES=$(wildcard $(MAIN_SRC)/*.c)
MAIN_HEADERS=$(wildcard $(MAIN_INC)/*.h)
MAIN_OUTPUTS=$(MAIN_SOURCES:$(MAIN_SRC)/%.c=$(MAIN_IR)/%.ir)
MAIN_BINARY=$(PK7_BIN)/lithmain.bin
MAIN_CFLAGS=-i$(MAIN_INC) -Dnull=NULL --sys-include $(LITHOS_INC)
MAIN_LFLAGS=-llithlib

LITHOS_IR=$(IR)/lithos
LITHOS_SRC=$(SRCDIR)/LithOS3/lithos_c/src
LITHOS_INC=$(SRCDIR)/LithOS3/lithos_c/inc
LITHOS_SOURCES=$(wildcard $(LITHOS_SRC)/*.c)
LITHOS_HEADERS=$(wildcard $(LITHOS_INC)/*.h)
LITHOS_OUTPUTS=$(LITHOS_SOURCES:$(LITHOS_SRC)/%.c=$(LITHOS_IR)/%.ir)
LITHOS_CFLAGS=-i$(LITHOS_INC)

CPK1_IR=$(IR)/cpk1
CPK1_SRC=$(SRCDIR)/Cpk1
CPK1_INC=$(CPK1_SRC)
CPK1_SOURCES=$(wildcard $(CPK1_SRC)/*.c)
CPK1_HEADERS=$(wildcard $(CPK1_INC)/*.h)
CPK1_OUTPUTS=$(CPK1_SOURCES:$(CPK1_SRC)/%.c=$(CPK1_IR)/%.ir)
CPK1_BINARY=pk7_cpk1/acs/lithcpk1.bin
CPK1_CFLAGS=-i$(CPK1_INC) --sys-include $(MAIN_INC) \
            --sys-include $(LITHOS_INC) -Dnull=NULL -DEXTERNAL_CODE
CPK1_LFLAGS=-llithlib -llithmain

LIB_STA =3000000000
MAIN_STA=3500000000
CPK1_STA=3700000000

## Targets
.PHONY: bin text clean

all: text bin
bin: $(LIB_BINARY) $(MAIN_BINARY) $(CPK1_BINARY)

text: compilefs.rb
	@cd filedata; ../compilefs.rb Directory.txt

clean:
	@rm -f $(MAIN_OUTPUTS) $(CPK1_OUTPUTS) $(LITHOS_OUTPUTS) $(LIB_OUTPUTS)

## .ir -> .bin
$(LIB_BINARY): $(LIB_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) --alloc-min Sta "" $(LIB_STA) $^ -o $@

$(MAIN_BINARY): $(MAIN_OUTPUTS) $(LITHOS_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) $(MAIN_LFLAGS) --alloc-min Sta "" $(MAIN_STA) $^ -o $@

$(CPK1_BINARY): $(CPK1_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) $(CPK1_LFLAGS) --alloc-min Sta "" $(CPK1_STA) $^ -o $@

## .c -> .ir
$(MAIN_IR)/%.ir: $(MAIN_SRC)/%.c $(MAIN_HEADERS) $(LITHOS_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(MAIN_CFLAGS) -c $< -o $@

$(LITHOS_IR)/%.ir: $(LITHOS_SRC)/%.c $(LITHOS_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(LITHOS_CFLAGS) -c $< -o $@

$(CPK1_IR)/%.ir: $(CPK1_SRC)/%.c $(MAIN_HEADERS) $(LITHOS_HEADERS) \
   $(CPK1_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(CPK1_CFLAGS) -c $< -o $@

$(IR)/libc.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libc -o $@

$(IR)/libGDCC.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libGDCC -o $@

## EOF
