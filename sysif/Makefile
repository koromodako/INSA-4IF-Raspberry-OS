#############################################################################
# 4IF SEA "SysIF" Makefile
#############################################################################

default: all

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

# options à passer au compilateur C si QEMU
CFLAGS_QEMU=-Wall -Werror -nostdlib -nostartfiles -ffreestanding -std=c99 -g -fomit-frame-pointer -nostartfiles -O0 -fdiagnostics-show-option -DQEMU
# options à passer au compilateur C
CFLAGS_STD=-Wall -Werror -nostdlib -nostartfiles -ffreestanding -std=c99 -g -fomit-frame-pointer -nostartfiles -O0 -fdiagnostics-show-option

all: kernel_for_qemu kernel_for_sdcard

kernel_for_qemu: CFLAGS=$(CFLAGS_QEMU) 
kernel_for_qemu: build/kernel.elf build/kernel.list

kernel_for_sdcard: CFLAGS=$(CFLAGS_STD) 
kernel_for_sdcard: build/kernel.elf build/kernel.img 

put_img_in_sdcard : kernel_for_sdcard kernel_for_sdcard
	cp build/kernel.img /media/antitoine/boot/kernel.img && umount /dev/mmcblk0p1 && umount /dev/mmcblk0p2

remake: clean all

# options à passer à la fois au compilateur C et à l'assembleur
COMMON_FLAGS=-mcpu=arm1176jzf-s

# Object files (excluding kmain)
OBJECTS=$(addsuffix .o,  $(addprefix build/, $(basename $(notdir $(wildcard src/*.[cs])))))

# non-default usage: "make KMAIN=test/my-kmain.c" 
KMAIN ?= ./kmain.c

# check whether kmain does exists. typically this would get triggered
# by a command like "make KMAIN=" (i.e. with no value for KMAIN)
ifneq "$(shell test -f ''${KMAIN} && echo yes || echo no)" "yes"
    $(error no such file: "${KMAIN}")
endif

# check whether we're dealing with a different kmain.c than last time:
# either updated/modified, or maybe another file, located somewhere else
ifneq "$(shell md5sum ${KMAIN})" "$(shell test -f build/kmain.md5 && cat build/kmain.md5)"
  # if so, then our kmain.o has to be recompiled
  .PHONY: build/kmain.o
endif

build:
	mkdir -p build

# compilation C vers ELF
build/%.o: src/%.c $(wildcard src/*.h) | build
	arm-none-eabi-gcc $(COMMON_FLAGS) ${CFLAGS} $< -c -o $@

# compilation du point d'entrée 
build/kmain.o: $(KMAIN) $(wildcard src/*.h)
	arm-none-eabi-gcc $(COMMON_FLAGS) ${CFLAGS} -I src $< -c -o $@
	@#"let's silently remember the checksum of the most recent kmain.c we compiled"
	@md5sum $(KMAIN) > build/kmain.md5

# assemblage .s vers ELF
build/%.o: src/%.s | build
	arm-none-eabi-as -g $(COMMON_FLAGS) $< -c -o $@

# édition de liens
build/kernel.elf: $(OBJECTS) build/kmain.o
	arm-none-eabi-ld $^ -o $@ -T src/sysif.ld -Map build/mapfile.map

# conversion de l'image pour transfert sur carte SD
build/kernel.img: build/kernel.elf
	arm-none-eabi-objcopy $^ -O binary $@

# désassemblage
build/kernel.list: build/kernel.elf
	arm-none-eabi-objdump -d -j .text -j .bss -j .stack -j .kernel_heap -j .user_stacks $< > $@

launch:
	make -C tools/

# nettoyage: effacer tous les fichiers générés
.PHONY:clean test launch
clean:
	rm -rf build
