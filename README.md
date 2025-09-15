# ext2boot

Simple tool for installing ext2/3/4 bootloader, based on libext2fs from e2fsprogs.

## Syntax

```ext2boot <filesystem> <bootloader binary>```

## Short description

This tool utilizes special inode (`EXT2_BOOT_LOADER_INO`) in ext2/3/4 filesystems/ To boot using installed boootloader required compatible partition boot record.
