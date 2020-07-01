// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

#include "../../src_fs/ouichefs.h"

MODULE_DESCRIPTION("ouichefs remove file politic when X% bloc are available");
MODULE_AUTHOR("ATLAOUI Tarik, CHECK Léo, ELIET Max");
MODULE_LICENSE("GPL");

static int (*clear_a_file_old)(struct dentry *root);

/**
 *Sub function used to go throug all directorys and all sub dirs
 *to find the bigest file for a remove operation 
 */
void __remove_the_bigger_file(struct dentry *root,
			      struct dentry **d_to_remove)
{
	struct list_head *p;
	struct dentry *d;
	struct inode *inode;
	/* for all this dentry children */
	list_for_each(p, &root->d_subdirs) {
		d = list_entry(p, struct dentry, d_child);
		inode = d->d_inode;
		/* and for every one of them */
		__remove_the_bigger_file(d, d_to_remove);

		/* Check that is not a directory */
		if (S_ISDIR(inode->i_mode))
			continue;
			
		if ((d->d_flags & DCACHE_DIRECTORY_TYPE) != 0)
			continue;

		if (inode == NULL)
			continue;
		/*selection check*/
		if (*d_to_remove == NULL || inode->i_size >
			(*d_to_remove)->d_inode->i_size) {

			if ((inode->i_state & I_DIRTY) == 0 &&
				inode->i_blocks > 0)

				*d_to_remove = d;
		}
	}
}
/**
 * remove the largest file if it exists
 */
int remove_the_bigger_file(struct dentry *root)
{
	int ret;
	struct dentry *d_to_remove = NULL;
	
	/* search */
	__remove_the_bigger_file(root, &d_to_remove);
	if (d_to_remove == NULL)
		return 1;

	pr_info("%s of size %lld in directory %s will be removed\n",
		d_to_remove->d_name.name, d_to_remove->d_inode->i_size,
	d_to_remove->d_parent->d_name.name);

	/* deletion */
	inode_lock(d_to_remove->d_parent->d_inode);
	ret = ouichefs_inode_ops.unlink
		(d_to_remove->d_parent->d_inode, d_to_remove);
	inode_unlock(d_to_remove->d_parent->d_inode);

	return ret;
}


static int __init hide_init(void)
{
	/* save the previous politic and switch */
	clear_a_file_old = ouichefs_politic.clear_a_file;
	ouichefs_politic.clear_a_file = remove_the_bigger_file;
	return 0;
}

static void __exit hide_exit(void)
{
	/*  restore the original politic */
	ouichefs_politic.clear_a_file = clear_a_file_old;
	pr_info("Module unloaded\n");
}

module_init(hide_init);
module_exit(hide_exit);
