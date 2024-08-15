savedcmd_arch/x86/coco/tdx/built-in.a := rm -f arch/x86/coco/tdx/built-in.a;  printf "arch/x86/coco/tdx/%s " tdx.o tdx-shared.o tdcall.o | xargs ar cDPrST arch/x86/coco/tdx/built-in.a
