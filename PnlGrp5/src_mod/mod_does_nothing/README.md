# mod_does_nothing

A module which redefines ouichefs politic suppression when X% blocks of superblock remain. It does not remove any file, but instead pr_info() a message on the terminal when X% or less blocks are available.

At rmmod, the module restores the original politic.
