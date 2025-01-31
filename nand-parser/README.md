# NAND Flash Parser

A utility tool to extract data from NAND flash dumps by locating the "FLSH" marker and extracting the following 32KB of data.

## Purpose
This tool is designed to:
- Find the "FLSH" marker in a binary NAND dump
- Extract exactly 32KB (32,768 bytes) of data following the marker
- Save the extracted data to a separate file
- Report the marker's offset in the original file

## Building
```bash
make        # Build the tool
make clean  # Clean build files
```

## Usage
```bash
./nand-extract <input_file> <output_file>
```

Arguments:
- `input_file`: Path to the NAND dump file to analyze
- `output_file`: Path where the extracted 32KB will be saved

Example:
```bash
./nand-extract nand_dump.bin extracted_data.bin
```

## Output
The tool provides the following information:
- Success/failure status
- Number of bytes extracted
- Offset where the FLSH marker was found (in hexadecimal)

Example output:
```
Successfully extracted 32768 bytes after FLSH marker
Marker found at offset: 0x17000
```
