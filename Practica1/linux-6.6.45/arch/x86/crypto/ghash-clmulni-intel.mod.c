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
	{ 0xe8b5c9f3, "cryptd_free_ahash" },
	{ 0xf0a5c2ff, "cryptd_alloc_ahash" },
	{ 0x73dd54eb, "irq_fpu_usable" },
	{ 0xc6a09d7b, "shash_ahash_update" },
	{ 0x25f30e5e, "crypto_ahash_setkey" },
	{ 0x928cad9b, "pcpu_hot" },
	{ 0xf9e493e6, "cryptd_ahash_child" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xbdbc9a07, "crypto_unregister_ahash" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0x4129f5ee, "kernel_fpu_begin_mask" },
	{ 0x38722f80, "kernel_fpu_end" },
	{ 0x27db38c7, "crypto_register_shash" },
	{ 0xabea0167, "crypto_unregister_shash" },
	{ 0x2972e0aa, "crypto_register_ahash" },
	{ 0x1db8419b, "cryptd_shash_desc" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x3862eada, "crypto_ahash_digest" },
	{ 0xa05d1f8b, "crypto_ahash_final" },
	{ 0xea01e791, "crypto_shash_final" },
	{ 0xb8350283, "shash_ahash_digest" },
	{ 0x2f69804e, "cryptd_ahash_queued" },
	{ 0x2ffb7d6f, "module_layout" },
};

MODULE_INFO(depends, "cryptd");

MODULE_ALIAS("cpu:type:x86,ven*fam*mod*:feature:*0081*");

MODULE_INFO(srcversion, "CC15CA7EE471EA85A026CE1");
