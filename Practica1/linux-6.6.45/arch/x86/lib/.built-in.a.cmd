savedcmd_arch/x86/lib/built-in.a := rm -f arch/x86/lib/built-in.a;  printf "arch/x86/lib/%s " msr-smp.o cache-smp.o msr.o msr-reg.o msr-reg-export.o hweight.o iomem.o iomap_copy_64.o | xargs ar cDPrST arch/x86/lib/built-in.a
