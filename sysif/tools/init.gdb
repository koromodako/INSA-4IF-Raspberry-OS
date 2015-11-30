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
b *sys_yieldto
b *do_sys_yieldto
b *irq_handler
b *user_process_1
b *user_process_2
b *user_process_3
b *elect

source utils.gdb

continue
