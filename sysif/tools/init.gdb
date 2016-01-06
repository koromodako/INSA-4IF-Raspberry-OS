# -*- mode:gdb-script -*-

layout split
focus cmd
winheight cmd 25
target remote:1234

set history filename ~/.gdb_history
set history save


b *after_kmain
b kernel_panic

b *reset_asm_handler
b *swi_handler
b *irq_handler
b *data_handler

b *user_process_1
b *sys_mmap
b *sys_munmap

b create_process
b kmain
b vmem_alloc_in_userland


source utils.gdb

continue
