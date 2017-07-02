## Compiler

CC=gdcc-cc
LD=gdcc-ld
MAKELIB=gdcc-makelib

## Directories

BIN=pk7/acs
SRC=pk7/lcode/Main
INC=$(SRC)
IR=ir

LITHOS_SRC=pk7/lcode/LithOS3/lithos_c/src
LITHOS_INC=pk7/lcode/LithOS3/lithos_c/inc

## Compiler flags

TARGET=--bc-target=ZDoom
LFLAGS=$(TARGET) --bc-zdacs-init-delay --bc-zdacs-chunk-STRE
CFLAGS=$(TARGET) -i$(LITHOS_INC) --sys-include $(LITHOS_INC)
ALLOCMIN=--alloc-min Sta ""

## Sources

MAIN_SOURCES= \
	$(SRC)/m_common.c \
	$(SRC)/m_list.c \
	$(SRC)/p_player.c \
	$(SRC)/w_world.c \
	$(SRC)/w_decorate.c \
	$(SRC)/p_pickups.c \
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
	$(SRC)/u_zoom.c \
	$(SRC)/u_homingrpg.c \
	$(SRC)/u_headsupdis2.c \
	$(SRC)/u_goldeneye.c \
	$(SRC)/p_cbigui.c \
	$(SRC)/p_bip.c \
	$(SRC)/p_weapons.c \
	$(SRC)/p_log.c \
	$(SRC)/p_shop.c \
	$(SRC)/p_games.c \
	$(SRC)/m_namegen.c \
	$(SRC)/m_file.c \
	$(SRC)/p_savedata.c \
	$(SRC)/p_shopdef.c \
	$(SRC)/p_data.c \
	$(SRC)/p_payout.c \
	$(SRC)/p_render.c \
	$(SRC)/p_save.c \
	$(SRC)/m_base64.c \
	$(SRC)/g_auto.c \
	$(SRC)/g_button.c \
	$(SRC)/g_checkbox.c \
	$(SRC)/g_preset.c \
	$(SRC)/g_scrollbar.c \
	$(SRC)/g_slider.c \
	$(SRC)/p_settings.c \
	$(SRC)/w_boss.c \
	$(SRC)/w_dialogue.c \
	$(SRC)/p_dialogue.c

MAIN_OUTPUTS=$(MAIN_SOURCES:$(SRC)/%.c=$(IR)/%.ir)
MAIN_HEADERS=$(wildcard $(INC)/*.h)

LITHOS_SOURCES=$(wildcard $(LITHOS_SRC)/*.c)
LITHOS_HEADERS=$(wildcard $(LITHOS_INC)/*.h)
LITHOS_OUTPUTS=$(LITHOS_SOURCES:$(LITHOS_SRC)/%.c=$(IR)/lithos_%.ir)

## all

all: $(BIN)/lithlib.bin $(BIN)/lithmain.bin

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
