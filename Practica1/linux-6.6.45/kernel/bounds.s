	.file	"bounds.c"
# GNU C11 (Ubuntu 13.2.0-23ubuntu4) version 13.2.0 (x86_64-linux-gnu)
#	compiled by GNU C version 13.2.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.26-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -mindirect-branch=thunk-extern -mindirect-branch-register -mindirect-branch-cs-prefix -mfunction-return=thunk-extern -mharden-sls=all -mrecord-mcount -mfentry -march=x86-64 -O2 -std=gnu11 -p -fshort-wchar -funsigned-char -fno-common -fno-PIE -fno-strict-aliasing -fcf-protection=none -falign-jumps=1 -falign-loops=1 -fno-asynchronous-unwind-tables -fno-jump-tables -fpatchable-function-entry=16,16 -fno-delete-null-pointer-checks -fno-allow-store-data-races -fstack-protector-strong -fno-omit-frame-pointer -fno-optimize-sibling-calls -ftrivial-auto-var-init=zero -fno-stack-clash-protection -fzero-call-used-regs=used-gpr -falign-functions=16 -fstrict-flex-arrays=3 -fno-strict-overflow -fstack-check=no -fconserve-stack -fsanitize=bounds-strict -fsanitize=shift -fsanitize=bool -fsanitize=enum -fstack-protector-strong
	.text
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.section	__patchable_function_entries,"awo",@progbits,.LPFE200
	.align 8
	.quad	.LPFE200
	.section	.text.startup
.LPFE200:
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	nop	
	.type	main, @function
main:
1:	call	__fentry__
	.section __mcount_loc, "a",@progbits
	.quad 1b
	.previous
	pushq	%rbp	#
	movq	%rsp, %rbp	#,
# kernel/bounds.c:19: 	DEFINE(NR_PAGEFLAGS, __NR_PAGEFLAGS);
#APP
# 19 "kernel/bounds.c" 1
	
.ascii "->NR_PAGEFLAGS $26 __NR_PAGEFLAGS"	#
# 0 "" 2
# kernel/bounds.c:20: 	DEFINE(MAX_NR_ZONES, __MAX_NR_ZONES);
# 20 "kernel/bounds.c" 1
	
.ascii "->MAX_NR_ZONES $5 __MAX_NR_ZONES"	#
# 0 "" 2
# kernel/bounds.c:22: 	DEFINE(NR_CPUS_BITS, order_base_2(CONFIG_NR_CPUS));
# 22 "kernel/bounds.c" 1
	
.ascii "->NR_CPUS_BITS $13 order_base_2(CONFIG_NR_CPUS)"	#
# 0 "" 2
# kernel/bounds.c:24: 	DEFINE(SPINLOCK_SIZE, sizeof(spinlock_t));
# 24 "kernel/bounds.c" 1
	
.ascii "->SPINLOCK_SIZE $4 sizeof(spinlock_t)"	#
# 0 "" 2
# kernel/bounds.c:26: 	DEFINE(LRU_GEN_WIDTH, order_base_2(MAX_NR_GENS + 1));
# 26 "kernel/bounds.c" 1
	
.ascii "->LRU_GEN_WIDTH $3 order_base_2(MAX_NR_GENS + 1)"	#
# 0 "" 2
# kernel/bounds.c:27: 	DEFINE(__LRU_REFS_WIDTH, MAX_NR_TIERS - 2);
# 27 "kernel/bounds.c" 1
	
.ascii "->__LRU_REFS_WIDTH $2 MAX_NR_TIERS - 2"	#
# 0 "" 2
# kernel/bounds.c:35: }
#NO_APP
	xorl	%eax, %eax	#
	popq	%rbp	#
	jmp	__x86_return_thunk
	.size	main, .-main
	.ident	"GCC: (Ubuntu 13.2.0-23ubuntu4) 13.2.0"
	.section	.note.GNU-stack,"",@progbits
