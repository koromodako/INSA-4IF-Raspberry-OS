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

b *swi_handler
b *irq_handler

b display_process_top_info
b display_process_right_top_text
b display_process_right_bottom_keyboard
b display_process_left_image

source utils.gdb

continue
