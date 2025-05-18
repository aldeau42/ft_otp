#include "ft_otp.h"

bool create_keyfile(const char *hex_key) {
    /*
    if (!is_valid_key((char *)hex_key)) {
        fprintf(stderr, "Error: Invalid key format.\n");
        return false;
    }
    */

    FILE *file = fopen("ft_otp.key", "w");
    if (!file) {
        perror("fopen");
        return false;
    }

    fwrite(hex_key, sizeof(char), strlen(hex_key), file);
    fclose(file);

    printf("Key saved to ft_otp.key successfully.\n");
    return true;
}

// Reads entire file content and returns a malloc'd string (must be freed by caller)
char *read_file_content(void) {
    FILE *file = fopen("ft_otp.key", "rb"); // Open in binary mode
    if (!file) {
        perror("fopen");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char *buffer = malloc(length + 1);
    if (!buffer) {
        perror("malloc");
        fclose(file);
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, length, file);
    fclose(file);

    buffer[read_bytes] = '\0';
    return buffer;
}

bool is_hex(char *key) {
    for (size_t i = 0; key[i] != '\0'; i++) {
        if (!((key[i] >= '0' && key[i] <= '9') ||
              (key[i] >= 'a' && key[i] <= 'f') ||
              (key[i] >= 'A' && key[i] <= 'F')))
            return false;
    }
    return true;
}

bool is_valid_key(char *key) {
    return is_hex(key) && strlen(key) >= MIN_KEY_LENGTH;
}

int hex_to_bytes(const char *hex, unsigned char *out, int max_len) {
    int len = strlen(hex) / 2;
    if (len > max_len) return -1;
    for (int i = 0; i < len; i++)
        sscanf(hex + 2 * i, "%2hhx", &out[i]);
    return len;
}

void generate_hotp(uint8_t *bin_key, uint64_t counter) {
    unsigned char counter_bytes[8];
    for (int i = 7; i >= 0; i--) {
        counter_bytes[i] = counter & 0xFF;
        counter >>= 8;
    }

    unsigned char hmac[EVP_MAX_MD_SIZE];
    unsigned int hmac_len;
    HMAC(EVP_sha1(), bin_key, 32, counter_bytes, 8, hmac, &hmac_len);

    int offset = hmac[hmac_len - 1] & 0x0F;
    uint32_t binary_code =
        ((hmac[offset] & 0x7F) << 24) |
        ((hmac[offset + 1] & 0xFF) << 16) |
        ((hmac[offset + 2] & 0xFF) << 8) |
        (hmac[offset + 3] & 0xFF);

    int otp = binary_code % 1000000;
    printf("OTP code: %06d\n", otp);
}

void generate_totp(void) {
    uint8_t bin_key[KEY_SIZE];
    char *hex_key = read_file_content();

    if (!hex_key || hex_to_bytes(hex_key, bin_key, KEY_SIZE) != KEY_SIZE) {
        fprintf(stderr, "Invalid hex key\n");
        free(hex_key);
        return;
    }
    uint64_t counter = time(NULL) / INTERVAL_TIME;
    generate_hotp(bin_key, counter);
    free(hex_key);
}

char *base32_encode(const uint8_t *bin_key) {
    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    char *out = malloc(53);
    if (!out) return NULL;

    int buffer = 0, bits_left = 0;
    size_t idx = 0;

    for (size_t i = 0; i < 32; ++i) {
        buffer = (buffer << 8) | bin_key[i];
        bits_left += 8;
        while (bits_left >= 5) {
            out[idx++] = alphabet[(buffer >> (bits_left - 5)) & 0x1F];
            bits_left -= 5;
        }
    }
    if (bits_left > 0) {
        buffer <<= (5 - bits_left);
        out[idx++] = alphabet[buffer & 0x1F];
    }
    out[idx] = '\0';
    return out;
}

void visualize_qr(const char *uri) {
    QRcode *qrcode = QRcode_encodeString(uri, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!qrcode) {
        fprintf(stderr, "QR generation failed\n");
        return;
    }
    for (int y = 0; y < qrcode->width; y++) {
        for (int x = 0; x < qrcode->width; x++) {
            printf(qrcode->data[y * qrcode->width + x] & 1 ? "██" : "  ");
        }
        printf("\n");
    }
    QRcode_free(qrcode);
}

void generate_qr(void) {
    uint8_t bin_key[KEY_SIZE];
    char *hex_key = read_file_content();

    if (!hex_key || hex_to_bytes(hex_key, bin_key, KEY_SIZE) != KEY_SIZE) {
        fprintf(stderr, "Invalid hex key\n");
        free(hex_key);
        return;
    }
    char *seed32 = base32_encode(bin_key);
    free(hex_key);

    const char *user = "user@ft_otp", *issuer = "ft_otp";
    char uri[256];
    snprintf(uri, sizeof(uri),
        "otpauth://totp/%s?secret=%s&issuer=%s&period=%d",
        user, seed32, issuer, INTERVAL_TIME);

    printf("OTP URI: %s\n\n", uri);
    visualize_qr(uri);
    free(seed32);
}
