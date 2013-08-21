#!/bin/sh
set -e

if test "$#" -ne 3 ; then
	echo "Usage: $0 KERNEL INITRAMFS OUTFILE"
	exit 1
fi

INITRAMFS="$(mktemp)"
pushd "$2" >/dev/null
find . | cpio -o -H newc 2>/dev/null | gzip > "$INITRAMFS"
popd >/dev/null

BOOTIMG="$(mktemp)"
mkbootimg --kernel "$1" --ramdisk "$INITRAMFS" -o "$BOOTIMG"

rm "$INITRAMFS"

./mangle_bootimg "$BOOTIMG" "$3"

rm "$BOOTIMG"
