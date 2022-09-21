CFLAGS += $(INCLUDES) $(DEFINES)

OBJS = $(addprefix $(BUILD)/, $(notdir %/$(subst .c,.o, $(SRCS))))

SUBMODULES = tinyusb

COBRA = cobra -f

all: $(BUILD)/$(BIN).elf $(BUILD)/$(BIN).hex $(BUILD)/$(BIN).bin $(BUILD)/$(BIN).uf2 size

$(BUILD)/$(BIN).elf: $(OBJS)
	@echo LD $@
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(BUILD)/$(BIN).hex: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O ihex $^ $@

$(BUILD)/$(BIN).bin: $(BUILD)/$(BIN).elf
	@echo OBJCOPY $@
	@$(OBJCOPY) -O binary $^ $@

$(BUILD)/$(BIN).uf2: $(BUILD)/$(BIN).bin
	@echo UF2CONV $@
	@$(UF2) $^ -co $@

.phony: $(SUBMODULES)
$(SUBMODULES):
	:

# 	git submodule update --init

install:
	@$(UF2) -D $(BUILD)/$(BIN).uf2

$(BUILD)/%.o: | $(SUBMODULES) directory
	@echo CC $@
	@$(CC) $(CFLAGS) $(filter %/$(subst .o,.c,$(notdir $@)), $(SRCS)) -c -o $@

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

DEPFILES := $(SRCS:%.c=$(BUILD)/%.d)

-include $(wildcard $(DEPFILES))
