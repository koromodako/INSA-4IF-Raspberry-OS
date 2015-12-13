# -*- mode:gdb-script -*-

layout split
focus cmd
winheight cmd 25
target remote:1234

set history filename ~/.gdb_history
set history save

b *reset_asm_handler
b *after_kmain
b kernel_panic

b do_sys_reboot
b do_sys_nop

b *kmain
b *swi_handler
b *irq_handler

source utils.gdb

continue
