savedcmd_arch/x86/kernel/msr.ko := ld -r -m elf_x86_64 -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T scripts/module.lds -o arch/x86/kernel/msr.ko arch/x86/kernel/msr.o arch/x86/kernel/msr.mod.o;  make -f ./arch/x86/Makefile.postlink arch/x86/kernel/msr.ko
