// SPDX-License-Identifier: GPL-2.0
/*
 * ouiche_fs - a simple educational filesystem for Linux
 *
 * Copyright (C) 2018  Redha Gouicem <redha.gouicem@lip6.fr>
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#define REMOVE1 "clear_a_file"
#define REMOVE2 "clear_a_file_in_dir"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/path.h>
#include <linux/namei.h>
#include <linux/namei.h>


#include "ouichefs.h"

/*
 * Mount a ouiche_fs partition
 */
struct dentry *ouichefs_mount(struct file_system_type *fs_type, int flags,
			      const char *dev_name, void *data)
{
	struct dentry *dentry = NULL;

	dentry = mount_bdev(fs_type, flags, dev_name, data,
			    ouichefs_fill_super);
	if (IS_ERR(dentry))
		pr_err("'%s' mount failure\n", dev_name);
	else
		pr_info("'%s' mount success\n", dev_name);

	return dentry;
}

/*
 * Unmount a ouiche_fs partition
 */
void ouichefs_kill_sb(struct super_block *sb)
{
	kill_block_super(sb);

	pr_info("unmounted disk\n");
}

static struct file_system_type ouichefs_file_system_type = {
	.owner = THIS_MODULE,
	.name = "ouichefs",
	.mount = ouichefs_mount,
	.kill_sb = ouichefs_kill_sb,
	.fs_flags = FS_REQUIRES_DEV,
	.next = NULL,
};

/*
 *	show on cat /sys/kernel/ouichefs/ouichefs
 */
static ssize_t ouichefs_show(struct kobject *kobj,
						struct kobj_attribute *attr,
						char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s", "Nothing to do for now.\n");
}

/*
 *	store on echo -n buf > /sys/kernel/ouichefs/ouichefs
 */
static ssize_t ouichefs_store(struct kobject *kobj,
						struct kobj_attribute *attr,
						const char *buf, size_t count)
{
	char *token, *cur = buf;
	char const *delim = " ";
	char *function = NULL, *dir = NULL;
	struct file *f = NULL;
	struct dentry *root = NULL;
	int i = 0;

	while ((token = strsep(&cur, delim))) {
		if (i == 1)
			dir = token;
		if (i == 0)
			function = token;
		i++;
	}

	if (i != 2) {
		pr_err("usage: <function> <dir_ouichefs>");
		return -1;
	}

	f = filp_open(dir, O_DIRECTORY, 0);
	if (IS_ERR(f)) {
		pr_err("%s does not exist", dir);
		filp_close(f, NULL);
		return -1;
	}

	if (strcmp(function, REMOVE1) == 0) {
		pr_info("%s\n", REMOVE1);
		root = get_root_dentry(f->f_path.dentry);
		pr_info("root = %s\n", root->d_name.name);
		pr_info("%d\n", ouichefs_politic.clear_a_file(root));
	} else if (strcmp(function, REMOVE2) == 0) {
		pr_info("%s\n", REMOVE2);
		ouichefs_politic.clear_a_file_in_dir
			(f->f_path.dentry, nb_file_in_dir(f->f_path.dentry));
	} else {
		pr_err("function %s not supported", function);
		filp_close(f, NULL);
		return -1;
	}

	filp_close(f, NULL);
	return count;


}

struct kobject *ouichefs_kobj;
struct kobj_attribute ouichefs_attribute =
	__ATTR_RW(ouichefs);

static int __init ouichefs_init(void)
{
	int ret;

	ret = ouichefs_init_inode_cache();
	if (ret) {
		pr_err("inode cache creation failed\n");
		goto end;
	}

	ret = register_filesystem(&ouichefs_file_system_type);
	if (ret) {
		pr_err("register_filesystem() failed\n");
		goto end;
	}

	ouichefs_kobj = kobject_create_and_add("ouichefs", kernel_kobj);
	if (!ouichefs_kobj)
		return -ENOMEM;

	ret = sysfs_create_file(ouichefs_kobj, &ouichefs_attribute.attr);
	if (ret)
		kobject_put(ouichefs_kobj);

	pr_info("module loaded\n");
end:
	return ret;
}

static void __exit ouichefs_exit(void)
{
	int ret;

	ret = unregister_filesystem(&ouichefs_file_system_type);
	if (ret)
		pr_err("unregister_filesystem() failed\n");

	ouichefs_destroy_inode_cache();

	kobject_put(ouichefs_kobj);

	pr_info("module unloaded\n");
}

module_init(ouichefs_init);
module_exit(ouichefs_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Redha Gouicem, <redha.gouicem@lip6.fr>");
MODULE_DESCRIPTION("ouichefs, a simple educational filesystem for Linux");
