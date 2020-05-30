#define pr_fmt(fmt) "ouichefs: " fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "ouichefs.h"

struct ouichefs_politic ouichefs_politic = {
	.clear_a_file = remove_lru_file,
	.clear_a_file_in_dir = remove_LRU_file_of_dir,
};
