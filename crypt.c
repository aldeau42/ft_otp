#include "ft_otp.h"

#define BLOCK_SIZE 16

// Convert hex string to bytes
void hex_to_bytes(const char *hex, unsigned char *out, size_t out_len) {
	for (size_t i = 0; i < out_len; ++i) {
		sscanf(hex + 2 * i, "%2hhx", &out[i]);
	}
}

// Converts 64-char hex string to 32-byte binary
int hex_to_bytes(const char *hex, unsigned char *out, int max_len) {
    int len = strlen(hex) / 2;
    if (len > max_len) return -1;
    for (int i = 0; i < len; i++) {
        sscanf(hex + 2 * i, "%2hhx", &out[i]);
    }
    return len;
}

// Standard HOTP implementation (RFC 4226)
int hotp_from_hex_key(const char *hex_key, uint64_t counter) {
    unsigned char key[32];
    if (hex_to_bytes(hex_key, key, 32) != 32) {
        fprintf(stderr, "Invalid hex key\n");
        return -1;
    }

    // Convert counter to 8-byte array (big-endian)
    unsigned char counter_bytes[8];
    for (int i = 7; i >= 0; i--) {
        counter_bytes[i] = counter & 0xFF;
        counter >>= 8;
    }

    // Calculate HMAC-SHA1
    unsigned char hmac[EVP_MAX_MD_SIZE];
    unsigned int hmac_len;
    HMAC(EVP_sha1(), key, 32, counter_bytes, 8, hmac, &hmac_len);

    // Dynamic Truncation
    int offset = hmac[hmac_len - 1] & 0x0F;
    uint32_t binary_code =
        ((hmac[offset] & 0x7F) << 24) |
        ((hmac[offset + 1] & 0xFF) << 16) |
        ((hmac[offset + 2] & 0xFF) << 8) |
        (hmac[offset + 3] & 0xFF);

    // Get 6-digit code
    return binary_code % 1000000;
}


