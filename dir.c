// SPDX-License-Identifier: GPL-2.0
/*
 * ouiche_fs - a simple educational filesystem for Linux
 *
 * Copyright (C) 2018 Redha Gouicem <redha.gouicem@lip6.fr>
 */
#define pr_fmt(fmt) "ouichefs: " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>

#include "ouichefs.h"

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

int remove_LRU_file_of_dir(struct dentry *dir, int nbFiles)
{
	struct inode *inode = NULL;
	struct list_head *p;
	struct dentry *d, *d_to_remove = NULL;

	list_for_each(p, &dir->d_subdirs) {
    	d = list_entry(p, struct dentry, d_child);
		inode = d->d_inode;
		if (d_to_remove == NULL
			|| inode->i_mtime.tv_sec < d_to_remove->d_inode->i_mtime.tv_sec) {
			d_to_remove = d;
			continue;
		}

	}
	pr_info("%s will be removed\n", d_to_remove->d_name.name);
	return ouichefs_inode_ops.unlink(dir->d_inode, d_to_remove);
}

void get_raw_path_of_files(struct dentry *dir, char **paths)
{
	struct ouichefs_inode_info *ci = OUICHEFS_INODE(dir->d_inode);
	struct super_block *sb = dir->d_inode->i_sb;
	struct buffer_head *bh = NULL;
	struct ouichefs_dir_block *dblock = NULL;
	struct ouichefs_file *f = NULL;
	struct inode *inode = NULL;
	char *pathdir, *tmp = kmalloc(PATH_MAX, GFP_KERNEL);
	char *pathfile;
	int i;

	bh = sb_bread(sb, ci->index_block);
	dblock = (struct ouichefs_dir_block *)bh->b_data;
	for (i = 0; i < OUICHEFS_MAX_SUBFILES; i++) {
		f = &dblock->files[i];
		if (!f->inode)
			break;
		inode = ouichefs_iget(sb, f->inode);
		pathdir = dentry_path_raw(dir, tmp, PATH_MAX);
		pathfile = kmalloc(OUICHEFS_MAX_PATH, GFP_KERNEL);
		strcpy(pathfile, pathdir);
		strcat(pathfile, "/");
		strcat(pathfile, f->filename);
		strcpy(paths[i], pathfile);
		iput(inode);
	}
}

/*
 * Iterate over the files contained in dir and commit them in ctx.
 * This function is called by the VFS while ctx->pos changes.
 * Return 0 on success.
 */
static int ouichefs_iterate(struct file *dir, struct dir_context *ctx)
{
	struct inode *inode = file_inode(dir);
	struct ouichefs_inode_info *ci = OUICHEFS_INODE(inode);
	struct super_block *sb = inode->i_sb;
	struct buffer_head *bh = NULL;
	struct ouichefs_dir_block *dblock = NULL;
	struct ouichefs_file *f = NULL;
	int i;

	/* Check that dir is a directory */
	if (!S_ISDIR(inode->i_mode))
		return -ENOTDIR;

	/*
	 * Check that ctx->pos is not bigger than what we can handle (including
	 * . and ..)
	 */
	if (ctx->pos > OUICHEFS_MAX_SUBFILES + 2)
		return 0;

	/* Commit . and .. to ctx */
	if (!dir_emit_dots(dir, ctx))
		return 0;

	/* Read the directory index block on disk */
	bh = sb_bread(sb, ci->index_block);
	if (!bh)
		return -EIO;
	dblock = (struct ouichefs_dir_block *)bh->b_data;

	/* Iterate over the index block and commit subfiles */
	for (i = ctx->pos - 2; i < OUICHEFS_MAX_SUBFILES; i++) {
		f = &dblock->files[i];
		if (!f->inode)
			break;
		if (!dir_emit(ctx, f->filename, OUICHEFS_FILENAME_LEN,
			      f->inode, DT_UNKNOWN))
			break;
		ctx->pos++;
	}

	brelse(bh);

	return 0;
}

const struct file_operations ouichefs_dir_ops = {
	.owner = THIS_MODULE,
	.iterate_shared = ouichefs_iterate,
};
