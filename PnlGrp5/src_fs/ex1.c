#define pr_fmt(fmt) "ouichefs: " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "ouichefs.h"

/*
 *	Get number of file in the inputed dentry directory
 */
int nb_file_in_dir(struct dentry *dir)
{
	struct inode *idir = dir->d_inode;
	struct ouichefs_inode_info *ci = OUICHEFS_INODE(idir);
	struct super_block *sb = idir->i_sb;
	struct buffer_head *bh = NULL;
	struct ouichefs_dir_block *dblock = NULL;
	struct ouichefs_file *f = NULL;
	int i;
	int cpt = 0;

	bh = sb_bread(sb, ci->index_block);
	dblock = (struct ouichefs_dir_block *)bh->b_data;
	for (i = 0; i < OUICHEFS_MAX_SUBFILES; i++) {
		f = &dblock->files[i];
		if (!f->inode)
			break;
		cpt++;
	}
	return cpt;
}

/*
 *	Remove least recently used file within the nbFiles files in the inputed
 *	dentry dir
 */
int remove_LRU_file_of_dir(struct dentry *dir, int nbFiles)
{
	struct inode *inode = NULL;
	struct list_head *p;
	struct dentry *d, *d_to_remove = NULL;
	int ret;

	list_for_each(p, &dir->d_subdirs) {
		d = list_entry(p, struct dentry, d_child);
		inode = d->d_inode;

		if ((d->d_flags & DCACHE_DIRECTORY_TYPE) != 0)
			continue;


		if (d_to_remove == NULL || inode->i_mtime.tv_sec <
			d_to_remove->d_inode->i_mtime.tv_sec) {

			if ((inode->i_state & I_DIRTY) == 0 &&
				inode->i_blocks > 0)

				d_to_remove = d;
		}
	}
	pr_info("%s will be removed\n", d_to_remove->d_name.name);
	inode_lock(dir->d_inode);
	ret = ouichefs_inode_ops.unlink(dir->d_inode, d_to_remove);
	inode_unlock(dir->d_inode);
	return ret;
}
/**
 *Sub function used to go throug all directorys and all sub dirs
 *to find the least recently used file
 */
void __remove_lru_file(struct dentry *root, struct dentry **d_to_remove)
{
	struct list_head *p;
	struct dentry *d;
	struct inode *inode;

	list_for_each(p, &root->d_subdirs) {
		d = list_entry(p, struct dentry, d_child);
		inode = d->d_inode;

		__remove_lru_file(d, d_to_remove);

		if ((d->d_flags & DCACHE_DIRECTORY_TYPE) != 0)
			continue;

		if (inode == NULL)
			continue;

		if (*d_to_remove == NULL || inode->i_mtime.tv_sec <
			(*d_to_remove)->d_inode->i_mtime.tv_sec) {

			if ((inode->i_state & I_DIRTY) == 0 &&
				inode->i_blocks > 0)

				*d_to_remove = d;
		}
	}
}

/*
 *	Remove least recently used file within every files in the inputed
 *	dentry dir and its subdirs.
 */
int remove_lru_file(struct dentry *root)
{
	int ret;
	struct dentry *d_to_remove = NULL;

	__remove_lru_file(root, &d_to_remove);
	if (d_to_remove == NULL)
		return 1;

	pr_info("%s in directory %s will be removed\n",
		d_to_remove->d_name.name, d_to_remove->d_parent->d_name.name);
	inode_lock(d_to_remove->d_parent->d_inode);
	ret = ouichefs_inode_ops.unlink
		(d_to_remove->d_parent->d_inode, d_to_remove);
	inode_unlock(d_to_remove->d_parent->d_inode);

	return ret;
}

/*
 *	Get the dentry root of the inputed dentry d.
 */
struct dentry *get_root_dentry(struct dentry *d)
{
	struct dentry *root = d;

	while (root->d_parent != root)
		root = root->d_parent;
	return root;
}
