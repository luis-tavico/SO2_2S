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
	{ 0x73dd54eb, "irq_fpu_usable" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x4129f5ee, "kernel_fpu_begin_mask" },
	{ 0x38722f80, "kernel_fpu_end" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x27db38c7, "crypto_register_shash" },
	{ 0x1936413e, "polyval_mul_non4k" },
	{ 0xabea0167, "crypto_unregister_shash" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x49dece42, "polyval_update_non4k" },
	{ 0x78d27962, "boot_cpu_data" },
	{ 0x2ffb7d6f, "module_layout" },
};

MODULE_INFO(depends, "polyval-generic");

MODULE_ALIAS("cpu:type:x86,ven*fam*mod*:feature:*0081*");

MODULE_INFO(srcversion, "72119333BB6554EB38929A6");
