savedcmd_arch/x86/crypto/aesni-intel.mod := printf '%s\n'   aesni-intel_asm.o aesni-intel_glue.o aesni-intel_avx-x86_64.o aes_ctrby8_avx-x86_64.o | awk '!x[$$0]++ { print("arch/x86/crypto/"$$0) }' > arch/x86/crypto/aesni-intel.mod