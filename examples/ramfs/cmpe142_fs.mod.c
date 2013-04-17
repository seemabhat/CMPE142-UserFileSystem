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
	{ 0xa8c16cf3, "module_layout" },
	{ 0x22ec33d5, "simple_rename" },
	{ 0x633ba0eb, "simple_rmdir" },
	{ 0x63502ec8, "simple_unlink" },
	{ 0x78144e57, "simple_link" },
	{ 0xde267b0c, "simple_lookup" },
	{ 0x26b1bb91, "generic_show_options" },
	{ 0x92e286e2, "simple_statfs" },
	{ 0x37ca23f, "generic_delete_inode" },
	{ 0x580decac, "simple_write_end" },
	{ 0x809f7dae, "simple_write_begin" },
	{ 0x22b10ab5, "simple_readpage" },
	{ 0xe7193d83, "generic_file_splice_read" },
	{ 0x5a34a8a0, "generic_file_splice_write" },
	{ 0x1392244f, "noop_fsync" },
	{ 0x9e7f9a45, "generic_file_mmap" },
	{ 0x9a3e1eef, "generic_file_aio_write" },
	{ 0x313c763f, "generic_file_aio_read" },
	{ 0x230f120f, "do_sync_write" },
	{ 0x4f0165a, "do_sync_read" },
	{ 0x54a8e933, "generic_file_llseek" },
	{ 0x34339fd1, "simple_getattr" },
	{ 0x5bd48bc9, "simple_setattr" },
	{ 0x8e6d05c2, "unregister_filesystem" },
	{ 0x27e1a049, "printk" },
	{ 0xdb531f6e, "register_filesystem" },
	{ 0xc735392f, "d_make_root" },
	{ 0x815b5dd4, "match_octal" },
	{ 0x44e9a829, "match_token" },
	{ 0x85df9b6c, "strsep" },
	{ 0x783c7933, "kmem_cache_alloc_trace" },
	{ 0x352091e6, "kmalloc_caches" },
	{ 0xb944a671, "save_mount_options" },
	{ 0xadaabe1b, "pv_lock_ops" },
	{ 0xd52bf1ce, "_raw_spin_lock" },
	{ 0x4fb27126, "d_instantiate" },
	{ 0x673ace2c, "inc_nlink" },
	{ 0x28805633, "simple_dir_operations" },
	{ 0x4a367be9, "page_symlink_inode_operations" },
	{ 0x8045f97f, "init_special_inode" },
	{ 0x34184afe, "current_kernel_time" },
	{ 0x35536dd9, "inode_init_owner" },
	{ 0xe953b21f, "get_next_ino" },
	{ 0xb2c8274d, "new_inode" },
	{ 0x6f3e5fcd, "mount_nodev" },
	{ 0x356a2d8c, "kill_litter_super" },
	{ 0x37a0cba, "kfree" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "DF7951B711BD729D6D64602");
