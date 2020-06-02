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
