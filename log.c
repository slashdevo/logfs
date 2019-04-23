/*
 * A  simple filesystem for the Linux kernel that logs everything.
 *
 * Initial author: Sankar P <sankar.curiosity@gmail.com>
 * Code Editors:
 * - Ahmed Elmayyah 		<a.elmayyah@gmail.com>
 * - AbdulRahman Yousry	<ar.yousry@gmail.com>
 * - Ahmed Mahmoud 		<ahmedmk1812@gmail.com>
 * License: GNU GPLv3  <https://www.gnu.org/licenses/>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
//If the kernel version is 3.11.0 or newer, use this definition
static int logfs_iterate(struct file *filePointer, struct dir_context *ctx)
#else
//Else use this one
static int logfs_readdir(struct file *filePointer, void *dir, filldir_t filldir)
#endif
{
	// Function skeleton for ls, to be added later.
}

const struct file_operations logfs_dir_operations = {
		  .owner = THIS_MODULE,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 11, 0)
		  .iterate = logfs_iterate,	//Set the read function file operation
#else
		  .readdir = simplefs_readdir,
#endif
};

struct dentry *logfs_lookup(struct inode *parent_inode, struct dentry *child_dentry, unsigned int flags)
{
	// Function skeleton used for the lookup function which is associated with dentrys,
	// to be added later.
}

static struct inode_operations logfs_inode_ops = {
		  .lookup = logfs_lookup,
};


/* This function creates, configures, and returns an inode
 * for the asked file or directory under the specified directory
 * on the specified device, managed by the specified superblock.
 *
 * 	struct super_block *sb					->		The superblock parameter.
 * 	const struct inode *newFile			->		The incoming file parameter.
 * 	umode_t mode								->		The file mode parameter (specifies the file type).
 * 	dev_t dev									->		The device parameter (specifies the device. i.e. hard drive or flash drive).
 * */
struct inode *logfs_get_inode(struct super_block *sb, const struct inode *newFile, umode_t mode, dev_t dev)
{
	struct inode *newInode = new_inode(sb); //Create a new inode.

	if (newInode)
	{
		newInode->i_ino = get_next_ino(); // i_ino is the serial number of the inode
		// read more here (https://www.win.tue.nl/~aeb/linux/lk/lk-8.html) Section 8.6
		inode_init_owner(newInode, newFile, mode);

		newInode->i_atime = newInode->i_mtime = newInode->i_ctime = current_time(newInode);
		// sets the last modified time for the new inode, which is the time of creation.

		switch (mode & S_IFMT) //S_IFMT is a kernel macro that is ANDed with a umode type to
									 // determine the type of the incoming file (*newFile) see "man stat"
									 // or read (https://www.gnu.org/software/libc/manual/html_node/Testing-File-Type.html)
		{
		case S_IFDIR:	//Case Directory
			/* i_nlink will be initialized to 1 in the inode_init_always function
			 * (that gets called inside the new_inode function),
			 * We change it to 2 for directories, for covering the "." entry */
			inc_nlink(newInode);
			break;
		case S_IFREG:	//Case Regular File, not handled yet
		case S_IFLNK:	//Case Link (like shortcut), not handled yet
		default:
			printk(KERN_ERR "logfs can create meaningful inode for only root directory at the moment\n");
			//Log error to kernel.
			return NULL;
		}
	}
	return newInode; //Returns the newly created inode.
}

/* This function, as the name implies, Makes the super_block valid and
 * fills filesystem specific information in the super block */
int logfs_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *newInode;

	/* A magic number that uniquely identifies our filesystem type */
	sb->s_magic = 0x02042019;

	newInode = logfs_get_inode(sb, NULL, S_IFDIR, 0);
	newInode->i_op = &logfs_inode_ops; //Setting inode operations
	newInode->i_fop = &logfs_dir_operations; //Setting the directory operations
	sb->s_root = d_make_root(newInode);
	//Used at mount time to make a root directory for the superblock
	if (!sb->s_root)
		return -ENOMEM; //Out of memory

	return 0;
}

//This function mounts the filesystem
static struct dentry *logfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data)
{
	struct dentry *ret;

	ret = mount_bdev(fs_type, flags, dev_name, data, logfs_fill_super);
	//Mount the block device

	if (unlikely(IS_ERR(ret)))
		printk(KERN_ERR "Error mounting logfs");
	else
		printk(KERN_INFO "logfs is succesfully mounted on [%s]\n", dev_name);
	return ret;
}

static void logfs_kill_superblock(struct super_block *s)
{
	printk(KERN_INFO "logfs superblock is destroyed. Unmount succesful.\n");
	/* This is just a dummy function as of now. As our filesystem gets matured,
	 * we will do more meaningful operations here */
	return;
}

//Identifying the filesystem type for the kernel
struct file_system_type logfs_fs_type = {
	.owner = THIS_MODULE,
	.name = "logfs",
	.mount = logfs_mount,
	.kill_sb = logfs_kill_superblock,
};

//Initialisation function
static int logfs_init(void)
{
	int ret;

	ret = register_filesystem(&logfs_fs_type); //Registers the filesystem to the kernel
	if (likely(ret == 0))
		printk(KERN_INFO "Sucessfully registered logfs\n");
	else
		printk(KERN_ERR "Failed to register logfs. Error:[%d]", ret);
	return ret;
}

static void logfs_exit(void)
{
	int ret;

	ret = unregister_filesystem(&logfs_fs_type); //Unregisters the filesystem to the kernel

	if (likely(ret == 0))
		printk(KERN_INFO "Sucessfully unregistered logfs\n");
	else
		printk(KERN_ERR "Failed to unregister logfs. Error:[%d]",
		       ret);
}

module_init(logfs_init);
module_exit(logfs_exit);

MODULE_LICENSE("GPLv3");
