savedcmd_arch/x86/lib/retpoline.o := gcc -Wp,-MMD,arch/x86/lib/.retpoline.o.d -nostdinc -I./arch/x86/include -I./arch/x86/include/generated  -I./include -I./arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I./include/uapi -I./include/generated/uapi -include ./include/linux/compiler-version.h -include ./include/linux/kconfig.h -D__KERNEL__ -fmacro-prefix-map=./= -D__ASSEMBLY__ -fno-PIE -m64 -DCC_USING_FENTRY -g -gdwarf-5    -c -o arch/x86/lib/retpoline.o arch/x86/lib/retpoline.S  ; ./tools/objtool/objtool --hacks=jump_label --hacks=noinstr --hacks=skylake --retpoline --rethunk --sls --stackval --static-call --uaccess --prefix=16   arch/x86/lib/retpoline.o

source_arch/x86/lib/retpoline.o := arch/x86/lib/retpoline.S

deps_arch/x86/lib/retpoline.o := \
    $(wildcard include/config/CALL_DEPTH_TRACKING) \
    $(wildcard include/config/RETHUNK) \
    $(wildcard include/config/CPU_SRSO) \
    $(wildcard include/config/CPU_UNRET_ENTRY) \
  include/linux/compiler-version.h \
    $(wildcard include/config/CC_VERSION_TEXT) \
  include/linux/kconfig.h \
    $(wildcard include/config/CPU_BIG_ENDIAN) \
    $(wildcard include/config/BOOGER) \
    $(wildcard include/config/FOO) \
  include/linux/stringify.h \
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
  arch/x86/include/asm/dwarf2.h \
  arch/x86/include/asm/cpufeatures.h \
  arch/x86/include/asm/required-features.h \
    $(wildcard include/config/X86_MINIMUM_CPU_FAMILY) \
    $(wildcard include/config/MATH_EMULATION) \
    $(wildcard include/config/X86_PAE) \
    $(wildcard include/config/X86_CMPXCHG64) \
    $(wildcard include/config/X86_CMOV) \
    $(wildcard include/config/X86_P6_NOP) \
    $(wildcard include/config/MATOM) \
    $(wildcard include/config/PARAVIRT_XXL) \
  arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/X86_UMIP) \
    $(wildcard include/config/X86_INTEL_MEMORY_PROTECTION_KEYS) \
    $(wildcard include/config/X86_5LEVEL) \
    $(wildcard include/config/PAGE_TABLE_ISOLATION) \
    $(wildcard include/config/ADDRESS_MASKING) \
    $(wildcard include/config/INTEL_IOMMU_SVM) \
    $(wildcard include/config/X86_SGX) \
    $(wildcard include/config/XEN_PV) \
    $(wildcard include/config/INTEL_TDX_GUEST) \
    $(wildcard include/config/X86_USER_SHADOW_STACK) \
  arch/x86/include/asm/alternative.h \
    $(wildcard include/config/SMP) \
    $(wildcard include/config/CALL_THUNKS) \
  arch/x86/include/asm/asm.h \
    $(wildcard include/config/KPROBES) \
  arch/x86/include/asm/extable_fixup_types.h \
  arch/x86/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  arch/x86/include/generated/asm/export.h \
  include/asm-generic/export.h \
  arch/x86/include/asm/nospec-branch.h \
    $(wildcard include/config/CALL_THUNKS_DEBUG) \
    $(wildcard include/config/NOINSTR_VALIDATION) \
    $(wildcard include/config/CPU_IBPB_ENTRY) \
  include/linux/static_key.h \
  include/linux/jump_label.h \
    $(wildcard include/config/JUMP_LABEL) \
    $(wildcard include/config/HAVE_ARCH_JUMP_LABEL_RELATIVE) \
  arch/x86/include/asm/jump_label.h \
    $(wildcard include/config/HAVE_JUMP_LABEL_HACK) \
  arch/x86/include/asm/nops.h \
  include/linux/objtool.h \
    $(wildcard include/config/FRAME_POINTER) \
  include/linux/objtool_types.h \
  arch/x86/include/asm/msr-index.h \
  include/linux/bits.h \
  include/linux/const.h \
  include/vdso/const.h \
  include/uapi/linux/const.h \
  include/vdso/bits.h \
  arch/x86/include/asm/unwind_hints.h \
  arch/x86/include/asm/orc_types.h \
  arch/x86/include/asm/percpu.h \
    $(wildcard include/config/X86_64_SMP) \
  arch/x86/include/asm/current.h \
  arch/x86/include/asm/frame.h \
  arch/x86/include/asm/GEN-for-each-reg.h \

arch/x86/lib/retpoline.o: $(deps_arch/x86/lib/retpoline.o)

$(deps_arch/x86/lib/retpoline.o):

arch/x86/lib/retpoline.o: $(wildcard ./tools/objtool/objtool)
#SYMVER __x86_indirect_call_thunk_r10 0x33d07fee
#SYMVER __x86_indirect_call_thunk_r11 0x2acb4eaf
#SYMVER __x86_indirect_call_thunk_r12 0x01e61d6c
#SYMVER __x86_indirect_call_thunk_r13 0x18fd2c2d
#SYMVER __x86_indirect_call_thunk_r14 0x57bcbaea
#SYMVER __x86_indirect_call_thunk_r15 0x4ea78bab
#SYMVER __x86_indirect_call_thunk_r8 0x107dd046
#SYMVER __x86_indirect_call_thunk_r9 0x0966e107
#SYMVER __x86_indirect_call_thunk_rax 0x67cc9453
#SYMVER __x86_indirect_call_thunk_rbp 0xad53a002
#SYMVER __x86_indirect_call_thunk_rbx 0x658a2a0a
#SYMVER __x86_indirect_call_thunk_rcx 0x6448403d
#SYMVER __x86_indirect_call_thunk_rdi 0x32de75a8
#SYMVER __x86_indirect_call_thunk_rdx 0x610756b8
#SYMVER __x86_indirect_call_thunk_rsi 0x2bb7c05d
#SYMVER __x86_indirect_call_thunk_rsp 0xb0b76945
#SYMVER __x86_indirect_jump_thunk_r10 0xee38a20e
#SYMVER __x86_indirect_jump_thunk_r11 0xf723934f
#SYMVER __x86_indirect_jump_thunk_r12 0xdc0ec08c
#SYMVER __x86_indirect_jump_thunk_r13 0xc515f1cd
#SYMVER __x86_indirect_jump_thunk_r14 0x8a54670a
#SYMVER __x86_indirect_jump_thunk_r15 0x934f564b
#SYMVER __x86_indirect_jump_thunk_r8 0xf5dcf929
#SYMVER __x86_indirect_jump_thunk_r9 0xecc7c868
#SYMVER __x86_indirect_jump_thunk_rax 0xba2449b3
#SYMVER __x86_indirect_jump_thunk_rbp 0x70bb7de2
#SYMVER __x86_indirect_jump_thunk_rbx 0xb862f7ea
#SYMVER __x86_indirect_jump_thunk_rcx 0xb9a09ddd
#SYMVER __x86_indirect_jump_thunk_rdi 0xef36a848
#SYMVER __x86_indirect_jump_thunk_rdx 0xbcef8b58
#SYMVER __x86_indirect_jump_thunk_rsi 0xf65f1dbd
#SYMVER __x86_indirect_jump_thunk_rsp 0x6d5fb4a5
#SYMVER __x86_indirect_thunk_r10 0x31549b2a
#SYMVER __x86_indirect_thunk_r11 0x284faa6b
#SYMVER __x86_indirect_thunk_r12 0x0362f9a8
#SYMVER __x86_indirect_thunk_r13 0x1a79c8e9
#SYMVER __x86_indirect_thunk_r14 0x55385e2e
#SYMVER __x86_indirect_thunk_r15 0x4c236f6f
#SYMVER __x86_indirect_thunk_r8 0xf90a1e85
#SYMVER __x86_indirect_thunk_r9 0xe0112fc4
#SYMVER __x86_indirect_thunk_rax 0x65487097
#SYMVER __x86_indirect_thunk_rbp 0xafd744c6
#SYMVER __x86_indirect_thunk_rbx 0x670ecece
#SYMVER __x86_indirect_thunk_rcx 0x66cca4f9
#SYMVER __x86_indirect_thunk_rdi 0x305a916c
#SYMVER __x86_indirect_thunk_rdx 0x6383b27c
#SYMVER __x86_indirect_thunk_rsi 0x29332499
#SYMVER __x86_indirect_thunk_rsp 0xb2338d81
#SYMVER __x86_return_thunk 0x5b8239ca
#SYMVER entry_untrain_ret 0x177a82c3
#SYMVER retbleed_untrain_ret 0x45fc19d2
#SYMVER srso_alias_untrain_ret 0xfeea58f2
