##
## Definitions
##

## Compiler

CC=gdcc-cc
LD=gdcc-ld
MAKELIB=gdcc-makelib

## Directories

BIN=../../acs
SRC=src
IR=bin

## IR Directories

IRLIB=$(IR)/lib

## Compiler flags

TARGET=--bc-target=ZDoom
LFLAGS=$(TARGET) --bc-zdacs-init-delay --bc-zdacs-chunk-STRE
CFLAGS=$(TARGET) -iinc -Dnull=NULL
FNMIN=--func-minimum ScriptI
ALLOCMIN=--alloc-min Sta ""

## Sources

MISC_SOURCES= \
	$(SRC)/lith_list.c   \
	$(SRC)/lith_common.c

MAIN_SOURCES= \
	$(SRC)/lith_player.c   \
	$(SRC)/lith_world.c    \
	$(SRC)/lith_hud.c      \
	$(SRC)/lith_log.c      \
	$(SRC)/lith_decorate.c \
	$(SRC)/lith_pickups.c  \
	$(SRC)/lith_upgrades.c \
	$(SRC)/lith_cbi.c      \
	$(SRC)/lith_cbi_gui.c  \
	$(SRC)/lith_bip.c

## Output

MISC_OUTPUT=$(MISC_SOURCES:$(SRC)/%.c=$(IR)/%.ir)
MAIN_OUTPUT=$(MAIN_SOURCES:$(SRC)/%.c=$(IR)/%.ir)

##
## Rules
##

## all

all: $(BIN)/lithlib.bin $(BIN)/lithmisc.bin $(BIN)/lithmain.bin

## bin/lith*.bin

$(BIN)/lithlib.bin: $(IRLIB)/libc.ir $(IRLIB)/libGDCC.ir
	$(LD) $(LFLAGS) $(FNMIN) 1200 $(ALLOCMIN) 300000000  $^ -o $@

$(BIN)/lithmisc.bin: $(MISC_OUTPUT)
	$(LD) $(LFLAGS) $(FNMIN) 1300 $(ALLOCMIN) 1000000000 $^ -o $@ \
		-llithlib

$(BIN)/lithmain.bin: $(MAIN_OUTPUT)
	$(LD) $(LFLAGS) $(FNMIN) 1400 $(ALLOCMIN) 3000000000 $^ -o $@ \
		-llithlib \
		-llithmisc

## ir/*.ir

$(IR)/%.ir: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $^ -o $@

## irlib/lib*.ir

$(IRLIB)/libc.ir:
	$(MAKELIB) $(TARGET) -c libc -o $@

$(IRLIB)/libGDCC.ir:
	$(MAKELIB) $(TARGET) -c libGDCC -o $@

## EOF
