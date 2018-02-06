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

LIB_INIT =--bc-zdacs-init-script-name '__lithlib.bin_init'
MAIN_INIT=--bc-zdacs-init-script-name '__lithmain.bin_init'

LIB_STA =70000
MAIN_STA=1400000

LIB_OUTPUTS=$(IR)/libc.ir $(IR)/libGDCC.ir
LIB_BINARY=$(PK7_BIN)/lithlib.bin
LIB_CFLAGS=$(LIB_INIT)
LIB_LFLAGS=$(LIB_INIT) --alloc-min Sta "" $(LIB_STA)

MAIN_IR=$(IR)/main
MAIN_SRC=$(SRCDIR)/Main
MAIN_INC=$(SRCDIR)/Headers
MAIN_SOURCES=$(wildcard $(MAIN_SRC)/*.c)
MAIN_HEADERS=$(wildcard $(MAIN_INC)/*.h)
MAIN_OUTPUTS=$(MAIN_SOURCES:$(MAIN_SRC)/%.c=$(MAIN_IR)/%.ir)
MAIN_BINARY=$(PK7_BIN)/lithmain.bin
MAIN_CFLAGS=-i$(MAIN_INC) $(MAIN_INIT) -Dnull=NULL --alloc-Aut 2150
MAIN_LFLAGS=-llithlib $(MAIN_INIT) --alloc-min Sta "" $(MAIN_STA)

DECOMPAT_INPUTS=$(MAIN_INC)/lith_weapons.h \
                $(MAIN_INC)/lith_pdata.h \
                $(MAIN_INC)/lith_wdata.h \
                $(MAIN_INC)/lith_upgradenames.h \
                $(MAIN_INC)/lith_scorenums.h

## Targets
.PHONY: bin dec clean text

all: dec text bin
bin: $(LIB_BINARY) $(MAIN_BINARY)

source/Headers/lith_weapons.h source/Main/p_weaponinfo.c: wepc.rb source/Weapons.txt
	@./wepc.rb source/Weapons.txt,source/Headers/lith_weapons.h,source/Main/p_weaponinfo.c

dec: decompat.rb $(DECOMPAT_INPUTS)
	@./decompat.rb $(DECOMPAT_INPUTS)

text: compilefs.rb
	@cd filedata; ../compilefs.rb Directory.txt

clean:
	@rm -f $(MAIN_OUTPUTS) $(LIB_OUTPUTS)

## .ir -> .bin
$(LIB_BINARY): $(LIB_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) $(LIB_LFLAGS) $^ -o $@

$(MAIN_BINARY): $(MAIN_OUTPUTS)
	@echo LD $@
	@$(LD) $(LFLAGS) $(MAIN_LFLAGS) $^ -o $@

## .c -> .ir
$(MAIN_IR)/%.ir: $(MAIN_SRC)/%.c $(MAIN_HEADERS)
	@echo CC $<
	@$(CC) $(CFLAGS) $(MAIN_CFLAGS) -DFileHash=$(shell ./strh.rb $<) -c $< -o $@

$(IR)/libc.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) $(LIB_CFLAGS) -c libc -o $@

$(IR)/libGDCC.ir:
	@echo MAKELIB $@
	@$(MAKELIB) $(TARGET) $(LIB_CFLAGS) -c libGDCC -o $@

## EOF
