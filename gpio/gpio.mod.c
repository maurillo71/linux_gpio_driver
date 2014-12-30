#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xd4733cff, "module_layout" },
	{ 0x5792fc3, "device_destroy" },
	{ 0x37a0cba, "kfree" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xd13b2b18, "class_destroy" },
	{ 0xce65a0a8, "__mutex_init" },
	{ 0xf550b75d, "cdev_del" },
	{ 0x71abefe1, "device_create" },
	{ 0xf6ede60b, "cdev_add" },
	{ 0xb9c78ada, "cdev_init" },
	{ 0x1df70be4, "__class_create" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0x6fe340df, "kmem_cache_alloc_trace" },
	{ 0x62c1ade7, "malloc_sizes" },
	{ 0x2da418b5, "copy_to_user" },
	{ 0x33d169c9, "_copy_from_user" },
	{ 0x281f751b, "mutex_trylock" },
	{ 0xcc99e735, "mutex_unlock" },
	{ 0xa5dd69ea, "mutex_lock" },
	{ 0x50eedeb8, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

