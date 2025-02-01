# NAND Flash Parser

A utility tool to extract data from NAND flash dumps by locating the "FLSH" marker and extracting the specified amount of data.

## Purpose
This tool is designed to:
- Find the "FLSH" marker in a binary NAND dump
- Extract the marker and the specified amount of data (default: 256KB/0x40000 bytes)
- Save both the marker and the extracted data to a separate file
- Report the marker's offset and total bytes extracted

## Building
```bash
make        # Build the tool
make clean  # Clean build files
```

## Usage
```bash
./nand_parser <input_file> <output_file> [--size <size>]
```

Arguments:
- `input_file`: Path to the NAND dump file to analyze
- `output_file`: Path where the extracted data will be saved
- `--size`: (Optional) Size of data to extract in hexadecimal (default: 0x40000)

Examples:
```bash
# Extract default size (256KB/0x40000)
./nand_parser nand_dump.bin extracted_data.bin

# Extract custom size (128KB/0x20000)
./nand_parser nand_dump.bin extracted_data.bin --size 0x20000
```

## Output
The tool provides the following information:
- Success/failure status
- Total bytes extracted (in hex and decimal)
- Offset where the FLSH marker was found (in hexadecimal)
- Specified extraction size

Example output:
```
Successfully extracted 0x40000 bytes (262144 bytes)
Marker found at offset: 0x17000
Extraction size: 0x40000
```

Note: The output file includes both the "FLSH" marker (4 bytes) and the specified amount of data after it.
