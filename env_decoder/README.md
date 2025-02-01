# U-Boot Environment Codec

## Background: U-Boot Environment Storage
- **NVRAM Implementation**: U-Boot environment variables are stored in a dedicated NVRAM partition within flash memory
- **Partition Layout**: Flash contains multiple partitions (e.g., bootloader, kernel, rootfs), with NVRAM being a small (~?KB) persistent storage area
- **Encrypted Storage**: 
  - Stores key-value pairs with values encrypted using AES-128
  - Keys remain in plaintext for accessibility
  - Encryption key is compiled into U-Boot binary

## Building
```bash
make  # Builds the codec
```

## Implementation Details
- Uses hardcoded 16-byte AES blob
- Custom character remapping
- Base64 decoding via custom implementation (uni_base64.c)
- AES-128 decryption
- No external dependencies

## Usage
```bash
# Encode plaintext into encrypted base64
./env-codec -e "your text"

# Decode encrypted base64 back to plaintext
./env-codec -d "base64_encoded_text"
```

Example:
```bash
$ ./env-codec -e "This is a test message for U-Boot environment"
Encoded output (base64):
KO0dCw3UzXGDLC7Lenu76Ow/AibOexW3M3JvzO4u5JKNuUlrNZBzgUN5uPqKKNn3
Output length: 64

$ ./env-codec -d "KO0dCw3UzXGDLC7Lenu76Ow/AibOexW3M3JvzO4u5JKNuUlrNZBzgUN5uPqKKNn3"
Decoded output:
This is a test message for U-Boot environment
Output length: 45 (reported: 45)
