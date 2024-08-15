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
	{ 0x5bf371fe, "crypto_unregister_alg" },
	{ 0x3ef051c8, "crypto_inc" },
	{ 0x73dd54eb, "irq_fpu_usable" },
	{ 0x37a0cba, "kfree" },
	{ 0x78580274, "simd_register_aeads_compat" },
	{ 0x78a16f48, "aes_decrypt" },
	{ 0x558b281d, "aes_expandkey" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xa10e7771, "crypto_register_alg" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x4129f5ee, "kernel_fpu_begin_mask" },
	{ 0x38722f80, "kernel_fpu_end" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x7cd8d75e, "page_offset_base" },
	{ 0x8e937a28, "skcipher_walk_virt" },
	{ 0xcab892c5, "simd_register_skciphers_compat" },
	{ 0x633e6e1b, "skcipher_walk_aead_encrypt" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x7b4929f7, "skcipher_walk_done" },
	{ 0x5a44f8cb, "__crypto_memneq" },
	{ 0xc62611e1, "scatterwalk_map_and_copy" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0xe00b1332, "simd_unregister_aeads" },
	{ 0xc34694f7, "simd_unregister_skciphers" },
	{ 0x78d27962, "boot_cpu_data" },
	{ 0xa3f12f69, "__crypto_xor" },
	{ 0x5a67b56f, "__static_call_update" },
	{ 0x80a095d8, "scatterwalk_ffwd" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x77e9eb37, "aes_encrypt" },
	{ 0xc390306c, "skcipher_walk_aead_decrypt" },
	{ 0x6b2b69f7, "static_key_enable" },
	{ 0x2ffb7d6f, "module_layout" },
};

MODULE_INFO(depends, "crypto_simd");

MODULE_ALIAS("cpu:type:x86,ven*fam*mod*:feature:*0099*");

MODULE_INFO(srcversion, "E28C017080DEA815293AD41");
