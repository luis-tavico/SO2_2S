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
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x127d83ea, "security_locked_down" },
	{ 0xeb6eb87, "add_taint" },
	{ 0xb9e276cf, "wrmsr_safe_regs_on_cpu" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x3a2d1dfa, "rdmsr_safe_regs_on_cpu" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x804af87c, "wrmsr_safe_on_cpu" },
	{ 0xf80be44e, "rdmsr_safe_on_cpu" },
	{ 0x802d59c4, "__register_chrdev" },
	{ 0x72d6fa77, "class_register" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x97fa317c, "__cpuhp_setup_state" },
	{ 0x601cc1cb, "class_unregister" },
	{ 0x19ac0ff7, "__cpuhp_remove_state" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x77bc13a0, "strim" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xa512b99d, "no_seek_end_llseek" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x511a6c2, "device_destroy" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x6ece9b5a, "device_create" },
	{ 0xfb384d37, "kasprintf" },
	{ 0xc6cbbc89, "capable" },
	{ 0x17de3d5, "nr_cpu_ids" },
	{ 0x5a5a2271, "__cpu_online_mask" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x1d24c881, "___ratelimit" },
	{ 0x928cad9b, "pcpu_hot" },
	{ 0x122c3a7e, "_printk" },
	{ 0x2ffb7d6f, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "542D3D1F9231017C1F6AA40");
