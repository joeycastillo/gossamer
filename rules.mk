CFLAGS += $(INCLUDES) $(DEFINES)

OBJS = $(addprefix $(BUILD)/, $(notdir $(subst .c,.o, $(SRCS))))

SUBMODULES = tinyusb

COBRA = cobra -f

ifndef EMSCRIPTEN
# Hardware build targets
ifeq ($(DFU), 1)
all: $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).hex $(BUILD)/$(BIN).bin $(BUILD)/$(BIN).dfu size
else
all: $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).hex $(BUILD)/$(BIN).bin $(BUILD)/$(BIN).uf2 size
endif
else
# Emscripten build targets
all: $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).html
endif

$(BUILD)/$(BIN).elf: $(OBJS)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/$(BIN).hex: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex $^ $@

$(BUILD)/$(BIN).bin: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O binary $^ $@

ifeq ($(CHIP),samd51)
$(BUILD)/$(BIN).uf2: $(BUILD)/$(BIN).bin
	@echo UF2CONV $@
	@$(UF2) $^ -b0x4000 -co $@
else
$(BUILD)/$(BIN).uf2: $(BUILD)/$(BIN).bin
	@echo UF2CONV $@
	@$(UF2) $^ -co $@
endif

$(BUILD)/$(BIN).dfu: $(BUILD)/$(BIN).elf
	@echo DFUCONV $@
	@$(DFU_CONV) $^ $@

# Emscripten HTML target
$(BUILD)/$(BIN).html: $(OBJS)
	@echo HTML $@
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@ \
		-s ASYNCIFY=1 \
		-s EXPORTED_RUNTIME_METHODS=lengthBytesUTF8,printErr \
		-s EXPORTED_FUNCTIONS=_main \
		--shell-file=./watch-library/simulator/shell.html

.PHONY: $(SUBMODULES) all clean size analyze install directory
$(SUBMODULES):
	:

# 	git submodule update --init

ifeq ($(DFU), 1)
install:
	@$(DFU_UTIL) -D $(BUILD)/$(BIN).dfu
	@echo "\033[0;97m\033[1mRemember to press RESET to run the newly installed code!\033[0m"
else
install:
	@$(UF2) -D $(BUILD)/$(BIN).uf2
endif

# Define a compile rule template
define compile_rule
$(BUILD)/$(notdir $(1:.c=.o)): $(1) | $(SUBMODULES) directory
	@echo CC $$@
	@$(CC) $(CFLAGS) $$< -c -o $$@
endef

# Generate a rule for each source file
$(foreach src,$(SRCS),$(eval $(call compile_rule,$(src))))

directory:
	@$(MKDIR) -p $(BUILD)

size: $(BUILD)/$(BIN).elf
	@echo size:
	@$(SIZE) -t $^

clean:
	@echo clean
	@-rm -rf $(BUILD)

analyze:
	@$(COBRA) basic $(INCLUDES) $(DEFINES) $(SRCS)

DEPFILES := $(addprefix $(BUILD)/, $(notdir $(SRCS:%.c=%.d)))

# Generate dependency files
$(BUILD)/%.d: %.c | directory
	@$(CC) $(CFLAGS) -MM -MT $(BUILD)/$(notdir $(<:.c=.o)) $< > $@


-include $(wildcard $(DEPFILES))
