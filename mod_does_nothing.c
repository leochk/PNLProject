// SPDX-License-Identifier: GPL-2.0
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

#include "ouichefs.h"

MODULE_DESCRIPTION("ouichefs remove file politic when X% bloc are available");
MODULE_AUTHOR("ATLAOUI Tarik, CHECK Léo, ELIET Max");
MODULE_LICENSE("GPL");

static int (*clear_a_file_old)(struct dentry *root);

int clear_nothing(struct dentry *root)
{
	pr_info("a module that is not removing a file\n");
	return 0;
}

static int __init hide_init(void)
{
	clear_a_file_old = ouichefs_politic.clear_a_file;
	ouichefs_politic.clear_a_file = clear_nothing;
	return 0;
}

static void __exit hide_exit(void)
{
	ouichefs_politic.clear_a_file = clear_a_file_old;
	pr_info("Module unloaded\n");
}

module_init(hide_init);
module_exit(hide_exit);
