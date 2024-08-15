#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x66283031, "x86_match_cpu" },
	{ 0x7c803277, "crypto_unregister_shashes" },
	{ 0x73dd54eb, "irq_fpu_usable" },
	{ 0x69acdf38, "memcpy" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x122c3a7e, "_printk" },
	{ 0x4129f5ee, "kernel_fpu_begin_mask" },
	{ 0x38722f80, "kernel_fpu_end" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x55385e2e, "__x86_indirect_thunk_r14" },
	{ 0x5006bce1, "crypto_register_shashes" },
	{ 0xcaa68533, "cpu_has_xfeatures" },
	{ 0xef013e87, "crypto_sha256_finup" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x66cca4f9, "__x86_indirect_thunk_rcx" },
	{ 0x56470118, "__warn_printk" },
	{ 0x78d27962, "boot_cpu_data" },
	{ 0x45ab0b8c, "crypto_sha256_update" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x2ffb7d6f, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("cpu:type:x86,ven*fam*mod*:feature:*0125*");
MODULE_ALIAS("cpu:type:x86,ven*fam*mod*:feature:*009C*");
MODULE_ALIAS("cpu:type:x86,ven*fam*mod*:feature:*0089*");

MODULE_INFO(srcversion, "21AEBF0634DFB3132B2ED83");
