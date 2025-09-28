# ---- Toolchain ----
TOOLROOT := /home/artj/avrgcc
BIN      := $(TOOLROOT)/bin
CC       := $(BIN)/avr-gcc
OBJCOPY  := $(BIN)/avr-objcopy
OBJDUMP  := $(BIN)/avr-objdump
SIZE     := $(BIN)/avr-size

# ---- Device/pack ----
MCU      := avr128db48
PACK_DIR := /home/artj/.mchp_packs/Microchip/AVR-Dx_DFP/2.7.321
# Used by avr-gcc to find device-specific specs/libs
DEV_B    := $(PACK_DIR)/gcc/dev/$(MCU)

# ---- Project ----
TARGET   ?= default
OUTDIR   := out/$(TARGET)
BUILDDIR := $(OUTDIR)/obj

# All C sources in the current dir (adjust if needed)
SOURCES  := $(wildcard *.c)
OBJECTS  := $(patsubst %.c,$(BUILDDIR)/%.o,$(SOURCES))

ELF := $(OUTDIR)/$(TARGET).elf
HEX := $(OUTDIR)/$(TARGET).hex
EEP := $(OUTDIR)/$(TARGET).eep
LSS := $(OUTDIR)/$(TARGET).lss
SREC:= $(OUTDIR)/$(TARGET).srec
USIG:= $(OUTDIR)/$(TARGET).usersignatures

# ---- Includes / Flags ----
# Your include paths:
INC_PACK  := $(PACK_DIR)/xc8/avr/include
INC_LOCAL := /home/artj/Documents/ECE3411uCLab/labs/include

CSTD   := -std=c11
WARN   := -Wall -Wextra -Wno-unknown-attributes -Wno-unknown-pragmas
DEFS   := -D__MPLAB_BUILD=1 -D__MPLAB_DEBUG=1 -D__DEBUG=1 -D__AVR128DB48__

# Match your requested flags (+ a few safe embedded defaults)
OPT    ?= -O1
CFLAGS := -gdwarf-2 -mmcu=$(MCU) -B$(DEV_B) $(CSTD) $(WARN) $(DEFS) $(OPT) \
          -ffunction-sections -fdata-sections \
          -I$(INC_PACK) -I$(INC_LOCAL)

# Linker flags with your --defsym settings (as MPLAB does)
LDFLAGS := -gdwarf-2 -mmcu=$(MCU) -B$(DEV_B) \
           -Wl,--gc-sections \
           -Wl,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1

# ---- Default target ----
.PHONY: all
all: $(ELF) $(HEX) $(EEP) $(LSS) $(SREC) $(USIG)
	@$(SIZE) $(ELF)

# ---- Build rules ----
$(OUTDIR) $(BUILDDIR):
	@mkdir -p $@

$(BUILDDIR)/%.o: %.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -MD -MP -c $< -o $@

$(ELF): $(OBJECTS) | $(OUTDIR)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(EEP): $(ELF)
	$(OBJCOPY) --only-section=.eeprom --change-section-lma .eeprom=0 \
	           --no-change-warnings -O ihex $< $@

$(LSS): $(ELF)
	$(OBJDUMP) --disassemble --wide --demangle --line-numbers \
	           --section-headers --source $< > $@

$(SREC): $(ELF)
	$(OBJCOPY) -O srec --remove-section=.eeprom --remove-section=.fuse \
	           --remove-section=.lock --remove-section=.signature $< $@

$(USIG): $(ELF)
	$(OBJCOPY) --only-section=.user_signatures \
	           --change-section-lma .user_signatures=0 \
	           --no-change-warnings -O ihex $< $@

# ---- Cleaning ----
.PHONY: clean
clean:
	rm -rf out

# ---- Dependencies ----
-include $(OBJECTS:.o=.d)
