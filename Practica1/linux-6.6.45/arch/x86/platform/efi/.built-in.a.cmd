savedcmd_arch/x86/platform/efi/built-in.a := rm -f arch/x86/platform/efi/built-in.a;  printf "arch/x86/platform/efi/%s " memmap.o quirks.o efi.o efi_64.o efi_stub_64.o efi_thunk_64.o runtime-map.o | xargs ar cDPrST arch/x86/platform/efi/built-in.a
