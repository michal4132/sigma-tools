#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#define FLSH_MARKER "FLSH"
#define DEFAULT_EXTRACT_SIZE (0x40000)  // 256KB default
#define READ_BUFFER_SIZE 4096

void print_usage(const char *program) {
    printf("NAND Flash Parser - Extract data after FLSH marker\n\n");
    printf("Usage: %s <input_file> <output_file> [--size <size>]\n", program);
    printf("  input_file:  Path to NAND dump file\n");
    printf("  output_file: Path to save extracted data\n");
    printf("  --size:      Size of data to extract in hex (default: 0x40000)\n");
    printf("\nExample:\n");
    printf("  %s nand_dump.bin extracted_data.bin --size 0x20000\n", program);
}

// Find FLSH marker in the buffer
size_t find_marker(const unsigned char *buffer, size_t size) {
    for (size_t i = 0; i <= size - 4; i++) {
        if (memcmp(buffer + i, FLSH_MARKER, 4) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}

int main(int argc, char *argv[]) {
    size_t extract_size = DEFAULT_EXTRACT_SIZE;
    char *input_file_path = NULL;
    char *output_file_path = NULL;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--size") == 0) {
            if (i + 1 < argc) {
                char *endptr;
                extract_size = strtoul(argv[i + 1], &endptr, 16);
                if (*endptr != '\0' || extract_size == 0) {
                    printf("Error: Invalid size parameter\n");
                    return 1;
                }
                i++; // Skip the size value
            } else {
                printf("Error: --size requires a value\n");
                return 1;
            }
        } else if (!input_file_path) {
            input_file_path = argv[i];
        } else if (!output_file_path) {
            output_file_path = argv[i];
        }
    }

    if (!input_file_path || !output_file_path) {
        print_usage(argv[0]);
        return 1;
    }

    FILE *input_file = fopen(input_file_path, "rb");
    if (!input_file) {
        printf("Error opening input file '%s': %s\n", input_file_path, strerror(errno));
        return 1;
    }

    FILE *output_file = fopen(output_file_path, "wb");
    if (!output_file) {
        printf("Error creating output file '%s': %s\n", output_file_path, strerror(errno));
        fclose(input_file);
        return 1;
    }

    unsigned char buffer[READ_BUFFER_SIZE];
    size_t bytes_read;
    size_t total_bytes_read = 0;
    size_t marker_position = SIZE_MAX;
    size_t bytes_after_marker = 0;
    int marker_found = 0;
    size_t absolute_marker_position = 0;

    // Search for FLSH marker
    while ((bytes_read = fread(buffer, 1, READ_BUFFER_SIZE, input_file)) > 0) {
        if (!marker_found) {
            marker_position = find_marker(buffer, bytes_read);
            if (marker_position != SIZE_MAX) {
                marker_found = 1;
                absolute_marker_position = total_bytes_read + marker_position;
                // Write FLSH marker first
                fwrite(buffer + marker_position, 1, 4, output_file);
                // Calculate remaining bytes in current buffer after marker
                size_t remaining = bytes_read - marker_position - 4;
                if (remaining > 0) {
                    // Adjust extract_size to account for FLSH marker
                    size_t to_write = (remaining > (extract_size - 4)) ? (extract_size - 4) : remaining;
                    fwrite(buffer + marker_position + 4, 1, to_write, output_file);
                    bytes_after_marker += to_write;
                }
            }
        } else {
            // Already found marker, continue writing data
            size_t remaining = (extract_size - 4) - bytes_after_marker;
            size_t to_write = (bytes_read > remaining) ? remaining : bytes_read;
            
            if (to_write > 0) {
                fwrite(buffer, 1, to_write, output_file);
                bytes_after_marker += to_write;
            }
            
            if (bytes_after_marker >= (extract_size - 4)) {
                break;
            }
        }
        
        total_bytes_read += bytes_read;
    }

    fclose(input_file);
    fclose(output_file);

    if (!marker_found) {
        printf("Error: FLSH marker not found in input file\n");
        remove(output_file_path);  // Delete output file
        return 1;
    }

    printf("Successfully extracted 0x%zX bytes (%zu bytes)\n", bytes_after_marker + 4, bytes_after_marker + 4);
    printf("Marker found at offset: 0x%zX\n", absolute_marker_position);
    printf("Extraction size: 0x%zX\n", extract_size);

    return 0;
}
