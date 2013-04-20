/* 
 *  cmpe142_fs.c -  Register and create a user file system
 */
 
#include <linux/kernel.h>	/* required for kernel modules */
#include <linux/module.h>	/* Specifically, a module */
#include <linux/init.h>	/* required for kernel initialization */
#include <linux/fs.h>	/* contain filesystem structs and macros */
#include <linux/mount.h>

#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/backing-dev.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/magic.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "internal.h"
#include "cmpe142fs.h"
#include <linux/mm.h>

//For Sockets
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>
#define NETLINK_USER 31
#define CMPE142_DEFAULT_MODE      0755

static const struct super_operations cmpe142_ops;
static const struct inode_operations cmpe142_dir_inode_operations;
struct sock *netlink_sk = NULL;

/* Initializing a file_system_type struct */
/*
static struct file_system_type cmpe142_fs_type = {
        .owner          = THIS_MODULE,
        .name           = "cmpe142",
        .fs_flags       = FS_REQUIRES_DEV,
        .kill_sb     = kill_block_super,
};
*/
static void cmpe142_kill_sb(struct super_block *sb)
{
        kfree(sb->s_fs_info);
        kill_litter_super(sb);
}

struct cmpe142_mount_opts {
        umode_t mode;
};

struct cmpe142_fs_info {
        struct cmpe142_mount_opts mount_opts;
};

enum {
        Opt_mode,
        Opt_err
};

static const match_table_t tokens = {
        {Opt_mode, "mode=%o"},
        {Opt_err, NULL}
};

const struct address_space_operations cmpe142_aops = {
        .readpage       = simple_readpage,
        .write_begin    = simple_write_begin,
        .write_end      = simple_write_end,
        //.set_page_dirty = __set_page_dirty_no_writeback,
};

const struct file_operations cmpe142_file_operations = {
        .read           = do_sync_read,
        .aio_read       = generic_file_aio_read,
        .write          = do_sync_write,
        .aio_write      = generic_file_aio_write,
        .mmap           = generic_file_mmap,
        .fsync          = noop_fsync,
        .splice_read    = generic_file_splice_read,
        .splice_write   = generic_file_splice_write,
        .llseek         = generic_file_llseek,
};

const struct inode_operations cmpe142_file_inode_operations = {
        .setattr        = simple_setattr,
        .getattr        = simple_getattr,
};

static int cmpe142_parse_options(char *data, struct cmpe142_mount_opts *opts)
{
        substring_t args[MAX_OPT_ARGS];
        int option;
        int token;
        char *p;

        opts->mode = CMPE142_DEFAULT_MODE;

        while ((p = strsep(&data, ",")) != NULL) {
                if (!*p)
                        continue;

                token = match_token(p, tokens, args);
                switch (token) {
                case Opt_mode:
                        if (match_octal(&args[0], &option))
                                return -EINVAL;
                        opts->mode = option & S_IALLUGO;
                        break;
                /*
                 * We might like to report bad mount options here;
                 * but traditionally ramfs has ignored all mount options,
                 * and as it is used as a !CONFIG_SHMEM simple substitute
                 * for tmpfs, better continue to ignore other mount options.
                 */
                }
        }

        return 0;
}

/* Initializing super block */
//int cmpe142_fill_super(struct super_block *sb, void *data, int silent);
int cmpe142_fill_super(struct super_block *sb, void *data, int silent)
{
        struct cmpe142_fs_info *fsi;
        struct inode *inode;
        int err;

        save_mount_options(sb, data);

        fsi = kzalloc(sizeof(struct cmpe142_fs_info), GFP_KERNEL);
        sb->s_fs_info = fsi;
        if (!fsi)
                return -ENOMEM;

        err = cmpe142_parse_options(data, &fsi->mount_opts);
        if (err)
                return err;

        sb->s_maxbytes          = MAX_LFS_FILESIZE;
        sb->s_blocksize         = PAGE_CACHE_SIZE;
        sb->s_blocksize_bits    = PAGE_CACHE_SHIFT;
        sb->s_magic             = RAMFS_MAGIC;
        sb->s_op                = &cmpe142_ops;
        sb->s_time_gran         = 1;

        inode = cmpe142_get_inode(sb, NULL, S_IFDIR | fsi->mount_opts.mode, 0);
        sb->s_root = d_make_root(inode);
        if (!sb->s_root)
                return -ENOMEM;

        return 0;
}

struct dentry *cmpe142_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data)
{
        return mount_nodev(fs_type, flags, data, cmpe142_fill_super);
}

/*
static struct dentry *rootfs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data)
{
        return mount_nodev(fs_type, flags|MS_NOUSER, data, cmpe142_fill_super);
}
*/

static struct file_system_type cmpe142_fs_type = {
        .name           = "cmpe142",
        .mount          = cmpe142_mount,
        .kill_sb        = cmpe142_kill_sb,
};

/*
static struct file_system_type rootfs_fs_type = {
        .name           = "rootfs",
        .mount          = rootfs_mount,
        .kill_sb        = kill_litter_super,
};
*/

/* Initializing inode */
//struct inode *cmpe142_get_inode(struct super_block *sb, const struct inode *dir,
 //        umode_t mode, dev_t dev);


static const struct super_operations cmpe142_ops = {
        .statfs         = simple_statfs,
        .drop_inode     = generic_delete_inode,
        .show_options   = generic_show_options,
};

static struct backing_dev_info cmpe142_backing_dev_info = {
        .name           = "cmpe142",
        .ra_pages       = 0,    /* No readahead */
        .capabilities   = BDI_CAP_NO_ACCT_AND_WRITEBACK |
                          BDI_CAP_MAP_DIRECT | BDI_CAP_MAP_COPY |
                          BDI_CAP_READ_MAP | BDI_CAP_WRITE_MAP | BDI_CAP_EXEC_MAP,
};

struct inode *cmpe142_get_inode(struct super_block *sb,
                                const struct inode *dir, umode_t mode, dev_t dev)
{
        struct inode * inode = new_inode(sb);

        if (inode) {
                inode->i_ino = get_next_ino();
                inode_init_owner(inode, dir, mode);
                inode->i_mapping->a_ops = &cmpe142_aops;
                inode->i_mapping->backing_dev_info = &cmpe142_backing_dev_info;
                mapping_set_gfp_mask(inode->i_mapping, GFP_HIGHUSER);
                mapping_set_unevictable(inode->i_mapping);
                inode->i_atime = inode->i_mtime = inode->i_ctime = CURRENT_TIME;
                switch (mode & S_IFMT) {
                default:
                        init_special_inode(inode, mode, dev);
                        break;
                case S_IFREG:
                        inode->i_op = &cmpe142_file_inode_operations;
                        inode->i_fop = &cmpe142_file_operations;
                        break;
                case S_IFDIR:
                        inode->i_op = &cmpe142_dir_inode_operations;
                        inode->i_fop = &simple_dir_operations;

                        /* directory inodes start off with i_nlink == 2 (for "." entry) */
                        inc_nlink(inode);
                        break;
                case S_IFLNK:
                        inode->i_op = &page_symlink_inode_operations;
                        break;
                }
        }
        return inode;
}

static int
cmpe142_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
        struct inode * inode = cmpe142_get_inode(dir->i_sb, dir, mode, dev);
        int error = -ENOSPC;

        if (inode) {
                d_instantiate(dentry, inode);
                dget(dentry);   /* Extra count - pin the dentry in core */
                error = 0;
                dir->i_mtime = dir->i_ctime = CURRENT_TIME;
        }
        return error;
}

static int cmpe142_mkdir(struct inode * dir, struct dentry * dentry, umode_t mode)
{
        int retval = cmpe142_mknod(dir, dentry, mode | S_IFDIR, 0);
        if (!retval)
                inc_nlink(dir);
        return retval;
}

static int cmpe142_create(struct inode *dir, struct dentry *dentry, umode_t mode, struct nameidata *nd)
{
        return cmpe142_mknod(dir, dentry, mode | S_IFREG, 0);
}

/* required for mounting */
//extern struct dentry *cmpe142_mount(struct file_system_type *fs_type,
 //        int flags, const char *dev_name, void *data);

static const struct inode_operations cmpe142_dir_inode_operations = {
        .create         = cmpe142_create,
        .lookup         = simple_lookup,
        .link           = simple_link,
        .unlink         = simple_unlink,
        //.symlink        = ramfs_symlink,
        .mkdir          = cmpe142_mkdir,
        .rmdir          = simple_rmdir,
        .mknod          = cmpe142_mknod,
        .rename         = simple_rename,
};


/* associating file operations */
extern const struct file_operations cmpe142_file_operations;

static void socket_receiver(struct sk_buff *skb)
{
	struct nlmsghdr *nlh;
	int pid;
	struct sk_buff *skb_out;
	int msg_size;
	char *msg="Hello from FS cmpe142";
	int res;

	printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

	msg_size=strlen(msg);

	nlh=(struct nlmsghdr*)skb->data;
	printk(KERN_INFO "Netlink received msg payload:%s\n",(char*)nlmsg_data(nlh));
	pid = nlh->nlmsg_pid; /*pid of sending process */

	skb_out = nlmsg_new(msg_size,0);

	if(!skb_out)
	{

	    printk(KERN_ERR "Failed to allocate new skb\n");
	    return;

	} 
	nlh=nlmsg_put(skb_out,0,0,NLMSG_DONE,msg_size,0);  
	NETLINK_CB(skb_out).dst_group = 0; /* not in mcast group */
	strncpy(nlmsg_data(nlh),msg,msg_size);

	res=nlmsg_unicast(netlink_sk,skb_out,pid);

	if(res<0)
	    printk(KERN_INFO "Error while sending bak to user\n");

	
}
int init_module()
{

	/**NETLINK_SOCKET_CREATION**/
	netlink_sk=netlink_kernel_create(&init_net, NETLINK_USER, 0, socket_receiver,NULL,THIS_MODULE);
	if(!netlink_sk)
	{

	    printk(KERN_ALERT "Error creating netlink socket.\n");
	    return -4;

	}

	/* registering filesystem */
	int register_fs_status = register_filesystem(&cmpe142_fs_type);
	
	/* checking if the registration was successful */
	if (register_fs_status != 0){
		printk(KERN_ALERT "Error: Could not register filesystem\n");
		return -ENOMEM;
	}

	printk(KERN_INFO "Filesystem registered\n");

	return 0;	/* success */
}

void cleanup_module()
{
        
	/**NETLINK_SOCKET_RELEASE**/
	netlink_kernel_release(netlink_sk);
	/* unregistering filesystem */
	unregister_filesystem(&cmpe142_fs_type);	
	printk(KERN_INFO "Filesystem removed\n");

}
