savedcmd_arch/x86/crypto/ghash-clmulni-intel.ko := ld -r -m elf_x86_64 -z noexecstack --no-warn-rwx-segments --build-id=sha1  -T scripts/module.lds -o arch/x86/crypto/ghash-clmulni-intel.ko arch/x86/crypto/ghash-clmulni-intel.o arch/x86/crypto/ghash-clmulni-intel.mod.o;  make -f ./arch/x86/Makefile.postlink arch/x86/crypto/ghash-clmulni-intel.ko
