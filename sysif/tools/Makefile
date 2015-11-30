
default:
	@echo "Starting qemu as a background process..."
	./run-qemu.sh &> qemu.log &
	@echo "done !"
	@echo "Starting GDB..."
	./run-gdb.sh
	@echo "GDB ended !"
	./kill-qemu.sh

.PHONY: default
