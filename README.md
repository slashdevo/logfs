# logfs
logfs is a simple kernel-space filesystem that logs everything. Heavily based on and inspired by ![this](github.com/psankar/simplefs/).

## Features
- Skeleton of the project has been set.
- You can only mount the filesystem, you do any other operations.


## Compilation

- Install linux kernel source:
##### Ubuntu and its derivatives
```# apt update && apt install linux-source```
##### Arch and its derivatives
```# pacman -Sy linux-headers```

- Run make:

Make sure that your working directory contains the files from this repo, then run ```make```.

## Testing

- Create a dummy image to mount.

```$ dd bs=1M count=100 if=/dev/zeo of=image```

- Create a mount directory.

```$ mkdir mount```
- Switch to root

```$ sudo su```
- Insert the kernel module.

```# insmod simplefs.ko```
- Check that it has been succesfuly inserted.

Run ```dmesg``` You should find: "Sucessfully registered logfs".
- Mount the image.

```mount -o loop -t logfs image mount/```

This mounts the image to the mount folder.

The ```-t``` option specifies the filesystem.
- Check that it has been succesfuly mounted

Run ```dmesg``` You should find: "logfs is succesfully mounted on [/dev/loop1]".
- Unmount the image

``` umount mount/```
Check with ```dmesg```, you should find "logfs  superblock is destroyed. Unmount succesful.".
- Remove the kernel module

```rmmod logfs.ko```
Check with ```dmesg```, you should find "Sucessfully unregistered logfs.".
