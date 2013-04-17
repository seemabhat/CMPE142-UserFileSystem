/* 
 *  cmpe142_fs.c -  Register and create a user file system
 */
 
#include <linux/kernel.h>	/* required for kernel modules */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/init.h>	/* required for kernel initialization */
#include <linux/fs.h>	/* contain filesystem structs and macros */
#include <linux/mount.h> /*required to mount the files system*/
#include <linux/err.h>


/* Initializing a file_system_type struct */
static struct file_system_type cmpe142_fs_type = {
        .owner          = THIS_MODULE,
        .name           = "cmpe142",
        .fs_flags       = FS_REQUIRES_DEV,
        .kill_sb     = kill_block_super,
};

/* Initializing super block */
//int cmpe142_fill_super(struct super_block *sb, void *data, int silent);
static struct super_block *cmpe142_super_block(struct super_block *, void *, int);

/* Initializing inode */
struct inode *cmpe142_get_inode(struct super_block *sb, const struct inode *dir,
         umode_t mode, dev_t dev);

/* required for mounting */
static struct vfsmount *cmpe142_mount = NULL;
/*extern struct dentry *cmpe142_mount(struct file_system_type *fs_type,
         int flags, const char *dev_name, void *data);*/

/* associating file operations */
extern const struct file_operations cmpe142_file_operations;

int init_module()
{
	/* registering filesystem */
	int err = register_filesystem(&cmpe142_fs_type);
	
	/* checking if the registration was successful */
	if (err != 0){
		printk(KERN_ALERT "Error: Could not register filesystem\n");
		return -ENOMEM;
	}
	else
	{
		//cmpe142_mount = vfs_kern_mount(&cmpe142_fs_type,0,"cmpe142",NULL);	
		/*if(IS_ERR(cmpe142_mount))
		{
			err = PTR_ERR(cmpe142_mount);
			printk(KERN_ALERT "Error: Could not mount Filesystem\n");
			unregister_filesystem(&cmpe142_fs_type);
			return err;
				
		}*/
		printk(KERN_INFO "Filesystem registered\n");
		return 0;	/* success */
	}
}

void cleanup_module()
{
        /* unregistering filesystem */
	unregister_filesystem(&cmpe142_fs_type);
	//mntput(cmpe142_mount);
	printk(KERN_INFO "Filesystem removed\n");

}

MODULE_LICENSE("GPL");
