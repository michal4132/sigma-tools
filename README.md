# Sigma Tools

A collection of tools for working with Sigma firmware and system images.

## Setup

1. Install required packages:
```bash
apt-get install sshpass
```

2. Configure your device:
```bash
# Copy the example config
cp device.mk.example work/device.mk

# Edit the config with your values
# - PARTITION_KEY
# - IP: Your device's IP address
# - PASSWORD: SSH password for root user
nano work/device.mk
```

## Building and Installing

### Building the Firmware

To build the firmware image, run:
```bash
make
```

This will create a `user0.img` file that can be installed on your device.

### Installing on Device

The installation process consists of two steps:

1. Upload the firmware to device:
```bash
make upload
```

2. Install and reboot (execute on device):
```bash
mtd write /tmp/user0.img USER0
reboot
```

> **Note:** Make sure your device configuration in `work/device.mk` is correct before uploading.

## Development and Build System

The project uses a Makefile-based build system for development. Here's what you need to know:

### Common Development Commands

- `make` or `make pack` - Builds the socketbridge binary and packs it into an encrypted image (default target)
- `make build` - Builds only the socketbridge binary
- `make upload` - Builds and uploads the encrypted image to the target device
- `make debug` - Build and upload playground binary
- `make clean` - Removes all build artifacts and generated files

### Build Requirements

- ARM GCC toolchain (placed in `work/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/`)
- Device configuration including partition key (configured in `work/device.mk`)

### Build Process Overview

1. The socketbridge binary is compiled using the ARM GCC toolchain
2. The binary is packed into a SquashFS image
3. The image is encrypted using the provided partition key
4. The resulting `user0.img` can be uploaded to the device

## Tools Overview

### Image Encryption Tools

#### pack_payload.sh
A script to pack and encrypt files into a SquashFS image. It creates an encrypted image from a directory of files using AES-CBC encryption.

```bash
Usage: ./pack_payload.sh [OPTIONS]

Options:
  -i, --input DIR     Input directory containing files to pack (default: user_rootfs)
  -o, --output FILE   Output encrypted image filename (default: user.img)
  -k, --key KEY       Encryption key in hex format (64 characters)
  -h, --help          Display this help message

Example:
  ./pack_payload.sh -i my_files -o encrypted.img
  ./pack_payload.sh --input my_files --output encrypted.img --key <your-key>
```

#### unpack_payload.sh
A script to decrypt and extract files from an encrypted SquashFS image.

```bash
Usage: ./unpack_payload.sh [OPTIONS]

Options:
  -i, --input FILE    Input encrypted image filename (default: user.img)
  -o, --output DIR    Output directory for extracted files (default: user_rootfs)
  -k, --key KEY       Encryption key in hex format (64 characters)
  -h, --help          Display this help message

Example:
  ./unpack_payload.sh -i encrypted.img -o extracted_files
  ./unpack_payload.sh --input encrypted.img --output extracted_files --key <your-key>
```

### Requirements

- Linux system with:
  - `squashfs-tools` (for mksquashfs)
  - `cryptsetup` (for encryption/decryption)
  - `xxd` (for key processing)
  - `sudo` privileges (for mounting and encryption operations)

### Security Notes

- The encryption key must be 64 characters long (256-bit in hexadecimal format)
- Keys are temporarily stored in a keyfile which is securely deleted after use
- All temporary files are cleaned up after operation
- The scripts require sudo privileges for mounting and encryption operations

## References

Links:
- [OpenWRT SSD20X uboot-sstar driver](https://github.com/wireless-tag-com/openwrt-ssd20x/blob/0462db78958d11cb937e662f56a93cdf30b92a59/18.06/package/sigmastar/uboot-sstar/src/drivers/mstar/spinand/drvSPINAND_api.c#L556)
