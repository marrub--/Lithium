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
INC=inc
IR=bin

## Compiler flags

TARGET=--bc-target=ZDoom
LFLAGS=$(TARGET) --bc-zdacs-init-delay --bc-zdacs-chunk-STRE
CFLAGS=$(TARGET) -iinc -Dnull=NULL
ALLOCMIN=--alloc-min Sta ""

## Sources

MAIN_SOURCES= \
	$(SRC)/lith_common.c \
	$(SRC)/lith_list.c \
	$(SRC)/lith_player.c \
	$(SRC)/lith_world.c \
	$(SRC)/lith_log.c \
	$(SRC)/lith_decorate.c \
	$(SRC)/lith_pickups.c \
	$(SRC)/lith_upgrades.c \
	$(SRC)/lith_upgr_7777777.c \
	$(SRC)/lith_upgr_adrenaline.c \
	$(SRC)/lith_upgr_cyberlegs.c \
	$(SRC)/lith_upgr_defensenuke.c \
	$(SRC)/lith_upgr_headsupdisp.c \
	$(SRC)/lith_upgr_implying.c \
	$(SRC)/lith_upgr_instadeath.c \
	$(SRC)/lith_upgr_jetbooster.c \
	$(SRC)/lith_upgr_lolsords.c \
	$(SRC)/lith_upgr_punctcannon.c \
	$(SRC)/lith_upgr_reactarmour.c \
	$(SRC)/lith_upgr_reflexwetw.c \
	$(SRC)/lith_upgr_riflemodes.c \
	$(SRC)/lith_upgr_unceunce.c \
	$(SRC)/lith_cbi.c \
	$(SRC)/lith_cbi_gui.c \
	$(SRC)/lith_bip.c \
	$(SRC)/lith_weapons.c \
	$(SRC)/lith_shop.c

## Output

MISC_OUTPUT=$(MISC_SOURCES:$(SRC)/%.c=$(IR)/%.ir)
MAIN_OUTPUT=$(MAIN_SOURCES:$(SRC)/%.c=$(IR)/%.ir)

##
## Rules
##

## all

all: $(BIN)/lithlib.bin $(BIN)/lithmain.bin

## bin/lith*.bin

$(BIN)/lithlib.bin: $(IR)/libc.ir $(IR)/libGDCC.ir
	$(LD) $(LFLAGS) $(ALLOCMIN) 300000000  $^ -o $@

$(BIN)/lithmain.bin: $(MAIN_OUTPUT)
	$(LD) $(LFLAGS) $(ALLOCMIN) 3000000000 $^ -o $@ -llithlib

## ir/*.ir

$(IR)/%.ir: $(SRC)/%.c $(wildcard $(INC)/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

## ir/lib*.ir

$(IR)/libc.ir:
	$(MAKELIB) $(TARGET) -c libc -o $@

$(IR)/libGDCC.ir:
	$(MAKELIB) $(TARGET) -c libGDCC -o $@

## EOF
