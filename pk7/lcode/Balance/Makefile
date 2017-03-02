##
## Definitions
##

## Compiler
ACC=gdcc-acc

## Directories
BIN_DOOM=../../acs
BIN_HTIC=../../filter/heretic/acs

## Sources
SRC_MAIN=../Main/inc/lith_upgradenames.h
SRC_DOOM=lith_balance_default.acs $(SRC_MAIN)
SRC_HTIC=lith_balance_heretic.acs $(SRC_MAIN)

## Output
OUT=trbohtlr.bin


##
## Rules
##

## all
all: $(BIN_DOOM)/$(OUT) $(BIN_HTIC)/$(OUT)

## */trbohtlr.bin
$(BIN_DOOM)/$(OUT): $(SRC_DOOM)
	$(ACC) $< -o $@

$(BIN_HTIC)/$(OUT): $(SRC_HTIC)
	$(ACC) $< -o $@

## EOF
