obj-m += nec_kbd_backlight.o

KDIR ?= /lib/modules/$(shell uname -r)/build
BUILD := $(CURDIR)/build

.PHONY: all clangd clean

all:
	mkdir -p $(BUILD)
	$(MAKE) -C $(KDIR) M=$(CURDIR) MO=$(BUILD) modules

clangd: all
	python3 $(KDIR)/scripts/clang-tools/gen_compile_commands.py -d $(BUILD) -o $(CURDIR)/compile_commands.json

clean:
	rm -rf $(BUILD)
