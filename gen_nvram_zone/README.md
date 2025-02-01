# NVRAM Zone Generator

A utility tool to generate NVRAM images with encrypted environment variables for Sigma firmware.

## Background: NVRAM Storage
- **NVRAM Format**: Uses a "FLSH" marker followed by encrypted key-value pairs
- **Encryption**: Values are encrypted using AES-128 and Base64 encoded
- **Storage Size**: Configurable size (typically 64KB-128KB) with header information
- **Configuration**: Uses a property file to define initial NVRAM contents

## Building
```bash
make        # Build the tool
make clean  # Clean build files
```

## Usage
```bash
./gen_nvram_zone --size=<size> --config=<config_file> --output=<output_file>

# Example:
./gen_nvram_zone --size=0x20000 --config=default.prop --output=nvram.img
```

### Parameters
- `--size`: Size of NVRAM image in hex (e.g., 0x20000 for 128KB)
- `--config`: Property file containing key-value pairs to store in NVRAM
- `--output`: Output file path for the generated NVRAM image

## Implementation Details
- Uses AES-128 encryption for value storage
- Base64 encoding via custom implementation (uni_base64.c)
- NVRAM format compatible with Sigma firmware
- Supports both encrypted and plaintext values
- Includes CRC verification
- No external dependencies

## Property File Format
```properties
# Comments start with #
key1=value1
key2=value2
# Values starting with ! are stored as plaintext
!plaintext_key=unencrypted_value
```

## Error Handling
- Validates input file format
- Checks for NVRAM size limits
- Verifies encryption/decryption operations
- Reports detailed error messages
