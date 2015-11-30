
default:
	@echo "Starting qemu as a background process..."
	./run-qemu.sh &> qemu.log &
	@echo "done !"
	@echo "Starting GDB..."
	./run-gdb.sh
	@echo "GDB ended !"
	@echo "Kill qemu process..."
	$(QEMU_PROC=$(echo $(ps -aux | grep -v grep | grep qemu-system-arm) | cut -d ' ' -f2) && kill -9 ${QEMU_PROC})
	@echo "done !"

.PHONY: default