// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "ouichefs.h"

int set_politic_clear_a_file(int (*new)(struct dentry *))
{
	ouichefs_politic.clear_a_file = new;
	return 0;
}
