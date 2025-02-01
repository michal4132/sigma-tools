#!/bin/bash

# Display help message
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Unpack files from an encrypted SquashFS image."
    echo
    echo "Options:"
    echo "  -i, --input FILE    Input encrypted image filename (default: user.img)"
    echo "  -o, --output DIR    Output directory for extracted files (default: user_rootfs)"
    echo "  -k, --key KEY       Encryption key in hex format (64 characters)"
    echo "  -h, --help          Display this help message"
    echo
    echo "Example:"
    echo "  $0 -i encrypted.img -o extracted_files"
    echo "  $0 --input encrypted.img --output extracted_files --key 1234..."
}

# Default configuration
ENCRYPTED_IMG="user.img"
EXTRACT_DIR="user_rootfs"
KEY=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -i|--input)
            ENCRYPTED_IMG="$2"
            shift 2
            ;;
        -o|--output)
            EXTRACT_DIR="$2"
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
MOUNT_POINT="/tmp/squashfs_mount"

# Validate inputs
if [ ! -f "$ENCRYPTED_IMG" ]; then
    echo "Error: Input file '$ENCRYPTED_IMG' does not exist"
    exit 1
fi

if [ ${#KEY} -ne 64 ]; then
    echo "Error: Encryption key must be 64 characters long (256-bit in hex)"
    exit 1
fi

if [ -d "$EXTRACT_DIR" ]; then
    if [ "$(ls -A "$EXTRACT_DIR")" ]; then
        read -r -p "Warning: Output directory '$EXTRACT_DIR' is not empty. Continue? [y/N] " response
        if [[ ! "$response" =~ ^[yY]$ ]]; then
            echo "Operation cancelled"
            exit 1
        fi
    fi
else
    mkdir -p "$EXTRACT_DIR"
fi

echo "Configuration:"
echo "- Input file: $ENCRYPTED_IMG"
echo "- Output directory: $EXTRACT_DIR"
echo "- Key length: ${#KEY} characters"
echo

# Create the keyfile
echo -n "$KEY" | xxd -r -p > "$KEYFILE"

# Decrypt the image
echo "Decrypting image..."
if ! sudo cryptsetup open --type plain --cipher aes-cbc-essiv:sha256 --key-file "$KEYFILE" "$ENCRYPTED_IMG" decrypted_user; then
    echo "Error: Failed to decrypt image. Check if the key is correct."
    rm -f "$KEYFILE"
    exit 1
fi

if ! sudo dd if=/dev/mapper/decrypted_user of="$SQUASHFS_IMG" bs=4M status=progress; then
    echo "Error: Failed to create SquashFS image"
    sudo cryptsetup close decrypted_user
    rm -f "$KEYFILE"
    exit 1
fi

sudo cryptsetup close decrypted_user

# Create mount point and extract files
echo "Extracting files..."
mkdir -p "$MOUNT_POINT"

if ! sudo mount "$SQUASHFS_IMG" "$MOUNT_POINT"; then
    echo "Error: Failed to mount SquashFS image"
    rm -f "$KEYFILE" "$SQUASHFS_IMG"
    rmdir "$MOUNT_POINT"
    exit 1
fi

if ! sudo cp -a "$MOUNT_POINT/." "$EXTRACT_DIR/"; then
    echo "Error: Failed to copy files"
    sudo umount "$MOUNT_POINT"
    rm -f "$KEYFILE" "$SQUASHFS_IMG"
    rmdir "$MOUNT_POINT"
    exit 1
fi

sudo umount "$MOUNT_POINT"

# Cleanup
rm -f "$KEYFILE" "$SQUASHFS_IMG"
rmdir "$MOUNT_POINT"

echo
echo "Success! Files extracted to: $EXTRACT_DIR"
echo "Extracted size: $(du -sh "$EXTRACT_DIR" | cut -f1)"
