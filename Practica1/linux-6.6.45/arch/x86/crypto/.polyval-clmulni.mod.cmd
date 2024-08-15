savedcmd_arch/x86/crypto/polyval-clmulni.mod := printf '%s\n'   polyval-clmulni_asm.o polyval-clmulni_glue.o | awk '!x[$$0]++ { print("arch/x86/crypto/"$$0) }' > arch/x86/crypto/polyval-clmulni.mod
