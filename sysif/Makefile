################################################################################
## 4IF SEA "SysIF" Makefile - Blehhhxanome - Raspberry OS
################################################################################


#### Regles par defaut #########################################################

default: all

#### Configuration #############################################################

BUILDDIR = build
SOURCEDIR = src
IMGDIR = img
TOOLSDIR = tools
LIBDIR = lib
SDCARDDIR = /media/antitoine/boot
UMOUNTDEV = /dev/mmcblk0p1 /dev/mmcblk0p2

# options a passer a la fois au compilateur C et a l'assembleur
COMMON_FLAGS=-mcpu=arm1176jzf-s
# options a passer au compilateur
CFLAGS_ALL=-Wall -Werror -nostdlib -nostartfiles -ffreestanding -std=c99 -g -fomit-frame-pointer -nostartfiles -O0 -fdiagnostics-show-option -fshort-wchar
# options a passer au compilateur C si QEMU
CFLAGS_QEMU=$(CFLAGS_ALL) -DQEMU
# options a passer au compilateur C
CFLAGS_STD=$(CFLAGS_ALL)


#### Variables calculees #######################################################

# non-default usage: "make KMAIN=test/my-kmain.c" 
KMAIN ?= ./kmain.c
# Object files (excluding kmain)
OBJECTS=$(addsuffix .o,  $(addprefix $(BUILDDIR)/, $(basename $(notdir $(wildcard $(SOURCEDIR)/**.[cs]))))) \
		$(addsuffix .o,  $(addprefix $(BUILDDIR)/, $(notdir $(wildcard $(IMGDIR)/**.pgm)))) \
		$(addsuffix .o,  $(addprefix $(BUILDDIR)/, $(notdir $(wildcard $(IMGDIR)/**.ppm))))

# check whether kmain does exists. typically this would get triggered
# by a command like "make KMAIN=" (i.e. with no value for KMAIN)
ifneq "$(shell test -f ''${KMAIN} && echo yes || echo no)" "yes"
    $(error no such file: "${KMAIN}")
endif


#### Phony #####################################################################

# check whether we're dealing with a different kmain.c than last time:
# either updated/modified, or maybe another file, located somewhere else
ifneq "$(shell md5sum ${KMAIN})" "$(shell test -f $(BUILDDIR)/kmain.md5 && cat $(BUILDDIR)/kmain.md5)"
  # if so, then our kmain.o has to be recompiled
  .PHONY: $(BUILDDIR)/kmain.o
endif

# nettoyage: effacer tous les fichiers generes
.PHONY:clean test launch

#### Regles ####################################################################

all: kernel_for_qemu kernel_for_sdcard

remake: clean all

clean:
	rm -rf $(BUILDDIR)

kernel_for_qemu: CFLAGS=$(CFLAGS_QEMU) 
kernel_for_qemu: $(BUILDDIR)/kernel.elf $(BUILDDIR)/kernel.list

kernel_for_sdcard: CFLAGS=$(CFLAGS_STD) 
kernel_for_sdcard: $(BUILDDIR)/kernel.elf $(BUILDDIR)/kernel.img 

put_img_in_sdcard: kernel_for_sdcard
	cp $(BUILDDIR)/kernel.img $(SDCARDDIR)/kernel.img && umount $(UMOUNTDEV)

launch: kernel_for_qemu
	make -C $(TOOLSDIR)/

## Build

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# compilation des images pgm et ppm vers pgm.o et ppm.o
$(BUILDDIR)/%.ppm.o: $(IMGDIR)/%.ppm
	arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm $< $@

$(BUILDDIR)/%.pgm.o: $(IMGDIR)/%.pgm
	arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm $< $@

# compilation C vers ELF
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c $(wildcard $(SOURCEDIR)/**.h) | $(BUILDDIR)
	arm-none-eabi-gcc $(COMMON_FLAGS) ${CFLAGS} $< -c -o $@

# compilation du point d'entree 
$(BUILDDIR)/kmain.o: $(KMAIN) $(wildcard $(SOURCEDIR)/**.h)
	arm-none-eabi-gcc $(COMMON_FLAGS) ${CFLAGS} -I $(SOURCEDIR) $< -c -o $@
	@#"let's silently remember the checksum of the most recent kmain.c we compiled"
	@md5sum $(KMAIN) > $(BUILDDIR)/kmain.md5

# assemblage .s vers ELF
$(BUILDDIR)/%.o: $(SOURCEDIR)/%.s | $(BUILDDIR)
	arm-none-eabi-as -g $(COMMON_FLAGS) $< -c -o $@

# edition de liens
$(BUILDDIR)/kernel.elf: $(OBJECTS) $(BUILDDIR)/kmain.o
	arm-none-eabi-ld --no-undefined $^ -o $@ -T $(SOURCEDIR)/sysif.ld -Map $(BUILDDIR)/mapfile.map -L$(LIBDIR) -lcsud

# conversion de l'image pour transfert sur carte SD
$(BUILDDIR)/kernel.img: $(BUILDDIR)/kernel.elf
	arm-none-eabi-objcopy $^ -O binary $@

# desassemblage
$(BUILDDIR)/kernel.list: $(BUILDDIR)/kernel.elf
	arm-none-eabi-objdump -d -t -j .text -j .bss -j .stack -j .kernel_heap -j .user_stacks $< > $@


#### Tests #####################################################################

test: 
	@echo   "+----------------------------------------------------------------------------------"
	@echo   "| N°: Test Description / Result"
	@echo   "+----------------------------------------------------------------------------------"
	@printf "|  1: Reboot Does Reboot... "													&& ./tools/run-test.sh test/kmain-reboot.c test/sys-reboot-does-reboot.gdb | tail -1
	@printf "|  2: Nop Does Return... "														&& ./tools/run-test.sh test/kmain-nop-reboot.c test/sys-nop-does-return.gdb | tail -1
	@printf "|  3: Set Time Passes Argument ... "											&& ./tools/run-test.sh test/kmain-settime.c test/sys-settime-passes-argument.gdb | tail -1
	@printf "|  4: Get Time Returns Value ... "												&& ./tools/run-test.sh test/kmain-gettime.c test/sys-gettime-returns-value.gdb | tail -1
	@printf "|  5: Yield To Jump To Dest... "												&& ./tools/run-test.sh test/kmain-yieldto.c test/sys-yieldto-jumps-to-dest.gdb | tail -1
	@printf "|  6: Yield To Preserves Locals ... "											&& ./tools/run-test.sh test/kmain-yieldto.c test/sys-yieldto-preserves-locals.gdb | tail -1
	@printf "|  7: Yield To Preserves Status Register ... " 								&& ./tools/run-test.sh test/kmain-yieldto.c test/sys-yieldto-preserves-status-register.gdb | tail -1
	@printf "|  8: Yield Round Robin Distinct Stacks ... " 									&& ./tools/run-test.sh test/kmain-yield-exit.c test/round-robin-distinct-stacks.gdb | tail -1
	@printf "|  9: Yield Round Robin Fairness ... " 										&& ./tools/run-test.sh test/kmain-yield-exit.c test/round-robin-fairness.gdb | tail -1
	@printf "| 10: Yield Round Robin Fairness Without Exit ... " 							&& ./tools/run-test.sh test/kmain-yield.c test/round-robin-fairness.gdb | tail -1
	@printf "| 11: Yield Round Robin Distinct Stacks Without Exit ... "						&& ./tools/run-test.sh test/kmain-yield.c test/round-robin-distinct-stacks.gdb | tail -1
	@printf "| 12: Yield Stack Scheduling With Time Interruption (it's a long test) ... "	&& ./tools/run-test.sh test/kmain-yield-multiprocess.c test/stack-scheduling-time-interruption.gdb | tail -1
	@echo   "+----------------------------------------------------------------------------------"