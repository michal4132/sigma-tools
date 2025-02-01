# Sigma Tools

A collection of tools for working with Sigma firmware and system images.

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
