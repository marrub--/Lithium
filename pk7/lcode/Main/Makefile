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
	$(SRC)/m_common.c \
	$(SRC)/m_list.c \
	$(SRC)/p_player.c \
	$(SRC)/w_world.c \
	$(SRC)/w_decorate.c \
	$(SRC)/m_pickups.c \
	$(SRC)/p_upgrades.c \
	$(SRC)/u_7777777.c \
	$(SRC)/u_adrenaline.c \
	$(SRC)/u_cyberlegs.c \
	$(SRC)/u_defensenuke.c \
	$(SRC)/u_headsupdisp.c \
	$(SRC)/u_implying.c \
	$(SRC)/u_instadeath.c \
	$(SRC)/u_jetbooster.c \
	$(SRC)/u_lolsords.c \
	$(SRC)/u_punctcannon.c \
	$(SRC)/u_reactarmor.c \
	$(SRC)/u_reflexwetw.c \
	$(SRC)/u_riflemodes.c \
	$(SRC)/u_unceunce.c \
	$(SRC)/u_vitalscan.c \
	$(SRC)/p_cbi.c \
	$(SRC)/p_bip.c \
	$(SRC)/p_weapons.c \
	$(SRC)/p_log.c \
	$(SRC)/p_shop.c \
	$(SRC)/p_games.c \
	$(SRC)/m_namegen.c \
	$(SRC)/m_netfile.c \
	$(SRC)/p_savedata.c \
	$(SRC)/p_shopdef.c \
	$(SRC)/p_data.c \
	$(SRC)/p_payout.c \
	$(SRC)/p_render.c \
	$(SRC)/p_save.c \
	$(SRC)/p_autogroup.c \
	$(SRC)/base64.c \
	$(SRC)/g_auto.c \
	$(SRC)/g_button.c \
	$(SRC)/g_checkbox.c \
	$(SRC)/g_preset.c \
	$(SRC)/g_scrollbar.c \
	$(SRC)/g_slider.c

## Output

MAIN_OUTPUT=$(MAIN_SOURCES:$(SRC)/%.c=$(IR)/%.ir)

##
## Rules
##

## all

all: $(BIN)/lithlib.bin $(BIN)/lithmain.bin

## acs/*.bin

$(BIN)/lithlib.bin: $(IR)/libc.ir $(IR)/libGDCC.ir
	$(LD) $(LFLAGS) $(ALLOCMIN) 300000000  $^ -o $@

$(BIN)/lithmain.bin: $(MAIN_OUTPUT)
	$(LD) $(LFLAGS) $(ALLOCMIN) 3000000000 $^ -o $@ -llithlib -ltrbohtlr

## ir/*.ir

$(IR)/%.ir: $(SRC)/%.c $(wildcard $(INC)/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

## ir/lib*.ir

$(IR)/libc.ir:
	$(MAKELIB) $(TARGET) -c libc -o $@

$(IR)/libGDCC.ir:
	$(MAKELIB) $(TARGET) -c libGDCC -o $@

## EOF
