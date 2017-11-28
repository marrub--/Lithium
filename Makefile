## Copyright © 2016-2017 Graham Sanderson
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
MAIN_INC=$(SRCDIR)/Headers
MAIN_SOURCES=$(wildcard $(MAIN_SRC)/*.c)
MAIN_HEADERS=$(wildcard $(MAIN_INC)/*.h)
MAIN_OUTPUTS=$(MAIN_SOURCES:$(MAIN_SRC)/%.c=$(MAIN_IR)/%.ir)
MAIN_BINARY=$(PK7_BIN)/lithmain.bin
MAIN_CFLAGS=-i$(MAIN_INC) -Dnull=NULL
MAIN_LFLAGS=-llithlib

DECOMPAT_INPUTS=$(wildcard $(SRCDIR)/DeCompat/*.dec) \
                $(MAIN_INC)/lith_weapons.h \
                $(MAIN_INC)/lith_pdata.h \
                $(MAIN_INC)/lith_wdata.h \
                $(MAIN_INC)/lith_lognames.h \
                $(MAIN_INC)/lith_upgradenames.h \
                $(MAIN_INC)/lith_scorenums.h

LIB_STA =3000000000
MAIN_STA=3500000000

## Targets
.PHONY: bin dec clean text

all: dec text bin
bin: $(LIB_BINARY) $(MAIN_BINARY)

dec: decompat.rb $(DECOMPAT_INPUTS)
	@./decompat.rb $(DECOMPAT_INPUTS)

text: compilefs.rb
	@cd filedata; ../compilefs.rb Directory.txt

clean:
	@rm -f $(MAIN_OUTPUTS) $(LIB_OUTPUTS)

## .ir -> .bin
$(LIB_BINARY): $(LIB_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) --alloc-min Sta "" $(LIB_STA) $^ -o $@

$(MAIN_BINARY): $(MAIN_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) $(MAIN_LFLAGS) --alloc-min Sta "" $(MAIN_STA) $^ -o $@

## .c -> .ir
$(MAIN_IR)/%.ir: $(MAIN_SRC)/%.c $(MAIN_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(MAIN_CFLAGS) -DFileHash=$(shell ./strh.rb $<) -c $< -o $@

$(IR)/libc.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libc -o $@

$(IR)/libGDCC.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) -c libGDCC -o $@

## EOF
