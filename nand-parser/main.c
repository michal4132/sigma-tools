#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#define FLSH_MARKER "FLSH"
#define EXTRACT_SIZE (32 * 1024)  // 32KB
#define READ_BUFFER_SIZE 4096

void print_usage(const char *program) {
    printf("NAND Flash Parser - Extract 32KB after FLSH marker\n\n");
    printf("Usage: %s <input_file> <output_file>\n", program);
    printf("  input_file:  Path to NAND dump file\n");
    printf("  output_file: Path to save extracted data\n");
    printf("\nExample:\n");
    printf("  %s nand_dump.bin extracted_data.bin\n", program);
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
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }

    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        printf("Error opening input file '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }

    FILE *output_file = fopen(argv[2], "wb");
    if (!output_file) {
        printf("Error creating output file '%s': %s\n", argv[2], strerror(errno));
        fclose(input_file);
        return 1;
    }

    unsigned char buffer[READ_BUFFER_SIZE];
    size_t bytes_read;
    size_t total_bytes_read = 0;
    size_t marker_position = SIZE_MAX;
    size_t bytes_after_marker = 0;
    int marker_found = 0;

    // Search for FLSH marker
    while ((bytes_read = fread(buffer, 1, READ_BUFFER_SIZE, input_file)) > 0) {
        if (!marker_found) {
            marker_position = find_marker(buffer, bytes_read);
            if (marker_position != SIZE_MAX) {
                marker_found = 1;
                // Calculate remaining bytes in current buffer after marker
                size_t remaining = bytes_read - marker_position - 4;
                if (remaining > 0) {
                    size_t to_write = (remaining > EXTRACT_SIZE) ? EXTRACT_SIZE : remaining;
                    fwrite(buffer + marker_position + 4, 1, to_write, output_file);
                    bytes_after_marker += to_write;
                }
            }
        } else {
            // Already found marker, continue writing data
            size_t remaining = EXTRACT_SIZE - bytes_after_marker;
            size_t to_write = (bytes_read > remaining) ? remaining : bytes_read;
            
            if (to_write > 0) {
                fwrite(buffer, 1, to_write, output_file);
                bytes_after_marker += to_write;
            }
            
            if (bytes_after_marker >= EXTRACT_SIZE) {
                break;
            }
        }
        
        total_bytes_read += bytes_read;
    }

    fclose(input_file);
    fclose(output_file);

    if (!marker_found) {
        printf("Error: FLSH marker not found in input file\n");
        remove(argv[2]);  // Delete output file
        return 1;
    }

    printf("Successfully extracted %zu bytes after FLSH marker\n", bytes_after_marker);
    printf("Marker found at offset: 0x%zX\n", total_bytes_read - bytes_read + marker_position);

    return 0;
}
