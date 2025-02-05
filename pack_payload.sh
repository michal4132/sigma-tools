#!/bin/bash

# Display help message
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Pack files into an encrypted SquashFS image."
    echo
    echo "Options:"
    echo "  -i, --input DIR     Input directory containing files to pack (default: user_rootfs)"
    echo "  -o, --output FILE   Output encrypted image filename (default: user.img)"
    echo "  -k, --key KEY       Encryption key in hex format (64 characters)"
    echo "  -h, --help          Display this help message"
    echo
    echo "Example:"
    echo "  $0 -i my_files -o encrypted.img"
    echo "  $0 --input my_files --output encrypted.img --key 1234..."
}

# Default configuration
ROOTFS_DIR="user_rootfs"
ENCRYPTED_IMG="user.img"
KEY=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -i|--input)
            ROOTFS_DIR="$2"
            shift 2
            ;;
        -o|--output)
            ENCRYPTED_IMG="$2"
            shift 2
            ;;
        -k|--key)
            KEY="$2"
            shift 2
            ;;
        *)
            echo "Error: Unknown option $1"
            show_help
            exit 1
            ;;
    esac
done

#######################
# Script Variables    #
#######################

KEYFILE="keyfile"
SQUASHFS_IMG="new_user.img"

# Validate inputs
if [ -z "$ROOTFS_DIR" ] || [ ! -d "$ROOTFS_DIR" ]; then
    echo "Error: Input directory '$ROOTFS_DIR' does not exist"
    exit 1
fi

if [ ${#KEY} -ne 64 ]; then
    echo "Error: Encryption key must be 64 characters long (256-bit in hex)"
    exit 1
fi

if [ -f "$ENCRYPTED_IMG" ]; then
    read -r -p "Warning: Output file '$ENCRYPTED_IMG' already exists. Overwrite? [y/N] " response
    if [[ ! "$response" =~ ^[yY]$ ]]; then
        echo "Operation cancelled"
        exit 1
    fi
fi

echo "Configuration:"
echo "- Input directory: $ROOTFS_DIR"
echo "- Output file: $ENCRYPTED_IMG"
echo "- Key length: ${#KEY} characters"
echo

# Create the keyfile
echo -n "$KEY" | xxd -r -p > "$KEYFILE"

# Create a new SquashFS file system
echo "Creating SquashFS image..."
if ! sudo mksquashfs "$ROOTFS_DIR" "$SQUASHFS_IMG" -comp xz; then
    echo "Error: Failed to create SquashFS image"
    rm -f "$KEYFILE"
    exit 1
fi

# Create encrypted image file with matching size
SQUASHFS_SIZE=$(stat -c %s "$SQUASHFS_IMG")
dd if=/dev/zero of="$ENCRYPTED_IMG" bs=1 count=0 seek="$SQUASHFS_SIZE"

# Set up loop device
LOOP_DEVICE=$(sudo losetup -f --show "$ENCRYPTED_IMG")

# Encrypt the SquashFS image
echo "Encrypting SquashFS image..."
if ! sudo cryptsetup open --type plain --cipher aes-cbc-essiv:sha256 --key-file "$KEYFILE" "$LOOP_DEVICE" new_user; then
    echo "Error: Failed to setup encryption"
    rm -f "$KEYFILE" "$SQUASHFS_IMG"
    exit 1
fi

if ! sudo dd if="$SQUASHFS_IMG" of="/dev/mapper/new_user" bs=4M status=progress; then
    echo "Error: Failed to create encrypted image"
    sudo cryptsetup close new_user
    sudo losetup -d "$LOOP_DEVICE"
    rm -f "$KEYFILE" "$SQUASHFS_IMG"
    exit 1
fi

sudo cryptsetup close new_user
sudo losetup -d "$LOOP_DEVICE"

# Cleanup
rm -f "$KEYFILE" "$SQUASHFS_IMG"

echo
echo "Success! Encrypted image created at: $ENCRYPTED_IMG"
echo "Image size: $(du -h "$ENCRYPTED_IMG" | cut -f1)"
