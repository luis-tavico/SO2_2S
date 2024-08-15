savedcmd_arch/x86/kernel/cpuid.mod := printf '%s\n'   cpuid.o | awk '!x[$$0]++ { print("arch/x86/kernel/"$$0) }' > arch/x86/kernel/cpuid.mod
