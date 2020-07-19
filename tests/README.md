# TEST

Theses bash scripts do everything for you. Make sure that a directory /share/PNLProject containing the project exists in your VM home directory.
Simply run theses tests by :

```sh
$ ./share/PNLProject/tests/<script>
```

## test_128_files_in_dir

Check if a file is removed when there are more than 128 files in a directory. The original politic is least recently used is removed.

## test_X_blocks_remaining

Check if a file is removed when superblock has X% blocks remaining. The original politic is least recently used is removed.

## test_ouichefs_sysfs

Check if ouichefs sysfs works correctly. When echo-ing <function> <dir> to /sys/kernel/ouichefs/ouichefs with correct arguments, it should trigger politic suppression.

## test_mod_does_nothing

/!\ Need test_ouichefs_sysfs to pass.
Tests mod_does_nothing correctness by using sysfs.
The inserted module should not remove any files. On rmmod, the scripts checks if the original politic is restored.

## test_mod_remove_bigger

/!\ Need test_ouichefs_sysfs to pass.
Tests mod_remove_bigger correctness by using sysfs. The inserted module should remove the bigger file in partition. On rmmod, the scripts checks if the original politic is restored.

## test_rand_files_in_subdirs

Create 10 directories in a ouichefs partition, and create 1000 random files
distributed in theses newly created directories.

## test_create_echo
create 4000 files with a rediraction , at the end the files 128 files will stil exists.

## test_create_touch
create 4000 files with touch function , at end the 128 files will stil exists as dir3.

## test_create_truncat
create 4000 files with truncat function to give each file a different size (larger at each iteration) 

## test_fileDir_exists
try to create the same files 130 times and ensure that the VM dont crash in that spécifique case