savedcmd_arch/x86/crypto/sha256-ssse3-asm.o := gcc -Wp,-MMD,arch/x86/crypto/.sha256-ssse3-asm.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -D__KERNEL__ -fmacro-prefix-map=./= -D__ASSEMBLY__ -fno-PIE -m64 -DCC_USING_FENTRY -g -gdwarf-5  -DMODULE  -c -o arch/x86/crypto/sha256-ssse3-asm.o arch/x86/crypto/sha256-ssse3-asm.S  ; ./tools/objtool/objtool --hacks=jump_label --hacks=noinstr --hacks=skylake --retpoline --rethunk --sls --stackval --static-call --uaccess --prefix=16   --module arch/x86/crypto/sha256-ssse3-asm.o

source_arch/x86/crypto/sha256-ssse3-asm.o := arch/x86/crypto/sha256-ssse3-asm.S

deps_arch/x86/crypto/sha256-ssse3-asm.o := \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/linkage.h \
    $(wildcard include/config/FUNCTION_ALIGNMENT) \
    $(wildcard include/config/ARCH_USE_SYM_ANNOTATIONS) \
  include/linux/compiler_types.h \
    $(wildcard include/config/DEBUG_INFO_BTF) \
    $(wildcard include/config/PAHOLE_HAS_BTF_TAG) \
    $(wildcard include/config/CC_IS_GCC) \
    $(wildcard include/config/X86_64) \
    $(wildcard include/config/ARM64) \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/CC_HAS_ASM_INLINE) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/MODVERSIONS) \
    $(wildcard include/config/64BIT) \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/OBJTOOL) \
  arch/x86/include/generated/asm/rwonce.h \
  include/asm-generic/rwonce.h \
  arch/x86/include/asm/linkage.h \
    $(wildcard include/config/X86_32) \
    $(wildcard include/config/CALL_PADDING) \
    $(wildcard include/config/RETHUNK) \
    $(wildcard include/config/RETPOLINE) \
    $(wildcard include/config/SLS) \
    $(wildcard include/config/FUNCTION_PADDING_BYTES) \
    $(wildcard include/config/UML) \
  arch/x86/include/asm/ibt.h \
    $(wildcard include/config/X86_KERNEL_IBT) \
  include/linux/types.h \
    $(wildcard include/config/HAVE_UID16) \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
    $(wildcard include/config/ARCH_32BIT_USTAT_F_TINODE) \
  include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  include/uapi/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/x86/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  include/linux/cfi_types.h \
    $(wildcard include/config/CFI_CLANG) \

arch/x86/crypto/sha256-ssse3-asm.o: $(deps_arch/x86/crypto/sha256-ssse3-asm.o)

$(deps_arch/x86/crypto/sha256-ssse3-asm.o):

arch/x86/crypto/sha256-ssse3-asm.o: $(wildcard ./tools/objtool/objtool)
