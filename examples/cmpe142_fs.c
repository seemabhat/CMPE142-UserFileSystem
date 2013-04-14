/* 
 *  procfs3.c -  create a "file" in /proc, use the file_operation way
 *  		to manage the file.
 */
 
#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/init.h>	/* Necessary because we use proc fs */
#include <linux/fs.h>	/* for copy_*_user */

//struct super_block sb;


static struct file_system_type cmpe142_fs_type = {
        .owner          = THIS_MODULE,
        .name           = "cmpe142",
        .fs_flags       = FS_REQUIRES_DEV,
        .kill_sb     = kill_block_super,
};


int cmpe142_fill_super(struct super_block *sb, void *data, int silent);

struct inode *cmpe142_get_inode(struct super_block *sb, const struct inode *dir,
         umode_t mode, dev_t dev);
extern struct dentry *cmpe142_mount(struct file_system_type *fs_type,
         int flags, const char *dev_name, void *data);

extern const struct file_operations cmpe142_file_operations;

int init_module()
{
	/* create the /proc file */
	int register_fs_status = register_filesystem(&cmpe142_fs_type);
	
	/* check if the /proc file was created successfuly */
	if (register_fs_status != 0){
		printk(KERN_ALERT "Error: Could not register filesystem\n");
		return -ENOMEM;
	}
	

	printk(KERN_INFO "Filesystem registered\n");

	return 0;	/* success */
}

void cleanup_module()
{
	unregister_filesystem(&cmpe142_fs_type);
	
	//remove_proc_entry(PROC_ENTRY_FILENAME, &proc_root);
	printk(KERN_INFO "Filesystem removed\n");

}
