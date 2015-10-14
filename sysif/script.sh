#!/bin/sh

./tools/run-test.sh test/kmain-reboot.c test/sys-reboot-does-reboot.gdb \
&& ./tools/run-test.sh test/kmain-nop-reboot.c test/sys-nop-does-return.gdb \
&& ./tools/run-test.sh test/kmain-settime.c test/sys-settime-passes-argument.gdb \
&& ./tools/run-test.sh test/kmain-gettime.c test/sys-gettime-returns-value.gdb \
&& ./tools/run-test.sh test/kmain-yieldto.c test/sys-yieldto-jumps-to-dest.gdb \
&& ./tools/run-test.sh test/kmain-yieldto.c test/sys-yieldto-preserves-locals.gdb \
&& ./tools/run-test.sh test/kmain-yieldto.c test/sys-yieldto-preserves-status-register.gdb
