savedcmd_arch/x86/purgatory/purgatory.chk := ld -m elf_x86_64 -z noexecstack --no-warn-rwx-segments  -e purgatory_start -z nodefaultlib arch/x86/purgatory/purgatory.ro -o arch/x86/purgatory/purgatory.chk
