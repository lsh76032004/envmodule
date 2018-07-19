#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
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
	{ 0xd29ace7d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x1d3e7692, __VMLINUX_SYMBOL_STR(misc_deregister) },
	{ 0xedc03953, __VMLINUX_SYMBOL_STR(iounmap) },
	{ 0xfc8f865a, __VMLINUX_SYMBOL_STR(kthread_stop) },
	{ 0x1ccdab42, __VMLINUX_SYMBOL_STR(misc_register) },
	{ 0xd6687bfa, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0xbb004d68, __VMLINUX_SYMBOL_STR(wake_up_process) },
	{ 0xdf49127f, __VMLINUX_SYMBOL_STR(kthread_create_on_node) },
	{ 0x79c5a9f0, __VMLINUX_SYMBOL_STR(ioremap) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x1cfb04fa, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x344b7739, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0x622598b1, __VMLINUX_SYMBOL_STR(init_wait_entry) },
	{ 0xa1c76e0a, __VMLINUX_SYMBOL_STR(_cond_resched) },
	{ 0xb3f7646e, __VMLINUX_SYMBOL_STR(kthread_should_stop) },
	{ 0x88260901, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x3d9835a6, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x2196324, __VMLINUX_SYMBOL_STR(__aeabi_idiv) },
	{ 0x2d3385d3, __VMLINUX_SYMBOL_STR(system_wq) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0xb2d48a2e, __VMLINUX_SYMBOL_STR(queue_work_on) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x8e865d3c, __VMLINUX_SYMBOL_STR(arm_delay_ops) },
	{ 0xd85cd67e, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "6081F283AB0EEBB8C147E0A");
