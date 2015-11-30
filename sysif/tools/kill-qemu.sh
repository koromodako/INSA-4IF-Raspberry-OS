#!/bin/bash

QEMU_PROC=$(echo $(ps -aux | grep -v grep | grep qemu-system-arm) | cut -d ' ' -f2)

echo "Killing Qemu (PID:${QEMU_PROC})"

kill -9 ${QEMU_PROC}

echo "Done !"
