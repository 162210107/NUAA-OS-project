rm -r ../../Build/*
riscv64-unknown-elf-g++ -nostdlib -fno-exceptions -fno-rtti -c ../../Kernel/Boot/main.cpp              -o ../../Build/main.o           -I"../../Include" -mcmodel=medany -std=c++17
riscv64-unknown-elf-g++ -nostdlib -fno-exceptions -fno-rtti -c ../../Kernel/Boot/Start.S               -o ../../Build/Start.o          -I"../../Include" -mcmodel=medany -std=c++17
riscv64-unknown-elf-g++ -nostdlib -fno-exceptions -fno-rtti -c ../../Kernel/Trap/Trap.cpp              -o ../../Build/Trap.o           -I"../../Include" -mcmodel=medany -std=c++17
riscv64-unknown-elf-g++ -nostdlib -fno-exceptions -fno-rtti -c ../../Kernel/Trap/TrapEntry.S           -o ../../Build/TrapEntry.o      -I"../../Include" -mcmodel=medany -std=c++17
riscv64-unknown-elf-g++ -nostdlib -fno-exceptions -fno-rtti -c ../../Kernel/Memory/PhysicalMemory.cpp  -o ../../Build/PhysicalMemory.o -I"../../Include" -mcmodel=medany -std=c++17
riscv64-unknown-elf-g++ -nostdlib -fno-exceptions -fno-rtti -c ../../Kernel/Library/libcpp.cpp         -o ../../Build/libcpp.o         -I"../../Include" -mcmodel=medany -std=c++17
riscv64-unknown-elf-ld -o ../../Build/OS16.elf -T ../../Linker/Kernel.ld ../../Build/*.o