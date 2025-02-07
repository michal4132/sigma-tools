#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "aes.h"
#include "uni_base64.h"
#include "../work/key.h"

int get_aes_key(const unsigned char *input, unsigned char *aes_key) {
    memset(aes_key, 0, 16);

    for (uint8_t i = 0; i < 16; i++) {
        unsigned char value = input[i];
        
        if (value < 10) {
            // Map 0-9 to ASCII digits '0'-'9'
            aes_key[i] = '0' + value;
        } else if (value >= 10 && value < 36) {
            // Map 10-35 to uppercase letters 'A'-'Z'
            aes_key[i] = 'A' + (value - 10);
        } else if (value >= 50 && value < 76) {
            // Map 50-75 to lowercase letters 'a'-'z'
            aes_key[i] = 'a' + (value - 50);
        } else if (value >= 101 && value < 116) {
            // Map 101-115 to special range 1
            aes_key[i] = value + 0xbc;
        } else if (value >= 121 && value < 128) {
            // Map 121-127 to special range 2
            aes_key[i] = value - 0x3f;
        } else if (value >= 128 && value < 134) {
            // Map 128-133 to special range 3
            aes_key[i] = value - 0x25;
        } else if (value >= 141 && value <= 144) {
            // Map 141-144 to special range 4
            aes_key[i] = value - 0x12;
        } else {
            return -1;
        }
    }

    return 0;
}

void print_usage(const char *program) {
    printf("U-Boot Environment Codec - Encode/Decode environment variables\n\n");
    printf("Usage: %s [-e|-d] <input_text>\n", program);
    printf("  -e: encode input text using AES-128 + Base64\n");
    printf("  -d: decode Base64 + AES-128 encrypted input\n");
    printf("\nExample:\n");
    printf("  %s -e \"Hello World\"\n", program);
    printf("  %s -d \"base64_encoded_text\"\n", program);
}

int encode_data(const char *input) {
    int ret;
    unsigned char aes_key[16];
    unsigned char *encode_output = NULL;
    unsigned int encode_len = 0;
    char *base64_output = NULL;
    size_t base64_len;
    
    // Get AES key from seed
    ret = get_aes_key(AES_BLOB, aes_key);
    if (ret != 0) {
        printf("Invalid AES Key Seed\n");
        return -1;
    }

    // Step 1: Encrypt data using AES
    ret = aes128_data_encode((const unsigned char*)input, strlen(input),
                            &encode_output, &encode_len, aes_key);
    if (ret != 0) {
        printf("AES encode failed, ret: %d\n", ret);
        return -1;
    }

    // Step 2: Encode to base64
    base64_len = ((encode_len + 2) / 3) * 4 + 1;  // Base64 length calculation
    base64_output = malloc(base64_len);
    if (NULL == base64_output) {
        printf("Memory allocation failed\n");
        free(encode_output);
        return -1;
    }

    char *result = tuya_base64_encode(encode_output, base64_output, encode_len);
    if (result == NULL) {
        printf("Base64 encode failed\n");
        free(encode_output);
        free(base64_output);
        return -1;
    }

    // Print results
    printf("Encoded output (base64):\n%s\n", base64_output);
    printf("Output length: %zu\n", strlen(base64_output));

    // Cleanup
    free(encode_output);
    free(base64_output);
    return 0;
}

int decode_data(const char *input) {
    int ret;
    unsigned char aes_key[16];
    unsigned char *decode_output = NULL;
    unsigned int decode_len = 0;
    unsigned char *base64_output = NULL;

    // Get AES key from seed
    ret = get_aes_key(AES_BLOB, aes_key);
    if (ret != 0) {
        printf("Invalid AES Key Seed\n");
        return -1;
    }

    printf("AES Key: %s\n", aes_key);

    // Step 1: Decode base64 input
    base64_output = malloc(strlen(input));
    if (NULL == base64_output) {
        printf("Memory allocation failed for %zu bytes\n", strlen(input));
        return -1;
    }

    ret = tuya_base64_decode(input, base64_output);
    if (ret <= 0) {
        printf("Base64 decode failed, ret: %d\n", ret);
        free(base64_output);
        return -1;
    }

    // Step 2: Decode data using derived AES key
    ret = aes128_data_decode(base64_output, ret, &decode_output, &decode_len, aes_key);
    free(base64_output);

    if (ret != 0 || decode_len <= 0) {
        printf("AES decode failed, ret: %d\n", ret);
        return -1;
    }

    // Ensure proper string termination
    if (strlen((char*)decode_output) > decode_len) {
        decode_output[decode_len] = 0;
    }

    // Print results
    printf("Decoded output:\n%s\n", decode_output);
    printf("Output length: %ld (reported: %d)\n", strlen((char*)decode_output), decode_len);

    // Cleanup
    if (decode_output != NULL) {
        free(decode_output);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3 || (strcmp(argv[1], "-e") != 0 && strcmp(argv[1], "-d") != 0)) {
        print_usage(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-e") == 0) {
        return encode_data(argv[2]);
    } else {
        return decode_data(argv[2]);
    }
}
