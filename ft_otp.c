#include "ft_otp.h"

void create_keyfile(char *hex_key) {
    if (!RAND_bytes(key->aes_key, sizeof(key->aes_key))) {
        fprintf(stderr, "Key generation failed\n");
        // handle error
    }
    printf("AES key: %s\n", key->aes_key);
    create_encrypted_keyfile(hex_key, key->aes_key);
    printf("AES key: %s\n", key->aes_key);
    return;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// AES ENC/DEC
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Encrypts plaintext and stores (IV + ciphertext)
int aes_encrypt(const unsigned char *plaintext, int plaintext_len,
                const unsigned char *aes_key, unsigned char *iv,
                unsigned char *ciphertext) {

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len, ciphertext_len;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aes_key, iv);

    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

// Decrypts (IV + ciphertext) back to plaintext
int aes_decrypt(const unsigned char *ciphertext, int ciphertext_len,
                const unsigned char *aes_key, unsigned char *iv,
                unsigned char *plaintext) {

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    int len, plaintext_len;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, aes_key, iv);

    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;

    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext_len;
}

void create_encrypted_keyfile(const char *hex_key, const unsigned char *aes_key) {
    unsigned char iv[IV_SIZE];
    unsigned char ciphertext[1024];

    RAND_bytes(iv, sizeof(iv)); // Generate random IV

    int ct_len = aes_encrypt((unsigned char *)hex_key, strlen(hex_key),
                             aes_key, iv, ciphertext);

    FILE *file = fopen("ft_otp.key", "wb");
    if (!file) {
        perror("fopen");
        return;
    }

    fwrite(iv, 1, IV_SIZE, file);
    fwrite(ciphertext, 1, ct_len, file);
    fclose(file);

    printf("Encrypted key written successfully to ft_otp.key!\n");
}

char *read_decrypted_keyfile(const unsigned char *aes_key) {
    FILE *file = fopen("ft_otp.key", "rb");
    if (!file) {
        perror("fopen");
        return NULL;
    }

    unsigned char iv[IV_SIZE];
    unsigned char ciphertext[1024];
    unsigned char plaintext[1024];

    fread(iv, 1, IV_SIZE, file);
    int ct_len = fread(ciphertext, 1, sizeof(ciphertext), file);
    fclose(file);

    int pt_len = aes_decrypt(ciphertext, ct_len, aes_key, iv, plaintext);
    plaintext[pt_len] = '\0'; // null-terminate

    return strdup((char *)plaintext);
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// OTP CODE
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
void generate_hotp(uint8_t *bin_key, uint64_t counter) {

    read_decrypted_keyfile(key->aes_key);

    // Convert counter to 8-byte array (big-endian)
    unsigned char counter_bytes[8];
    for (int i = 7; i >= 0; i--) {
        counter_bytes[i] = counter & 0xFF;
        counter >>= 8;
    }

    // Calculate HMAC-SHA1
    unsigned char hmac[EVP_MAX_MD_SIZE];
    unsigned int hmac_len;
    HMAC(EVP_sha1(), bin_key, 32, counter_bytes, 8, hmac, &hmac_len);

    // Dynamic Truncation
    int offset = hmac[hmac_len - 1] & 0x0F;
    uint32_t binary_code =
        ((hmac[offset] & 0x7F) << 24) |
        ((hmac[offset + 1] & 0xFF) << 16) |
        ((hmac[offset + 2] & 0xFF) << 8) |
        (hmac[offset + 3] & 0xFF);

    int otp = binary_code % 1000000;
    // Get 6-digit code
    if (otp >= 0)
        printf("OTP code: %06d\n", otp);
    return;
}
/*
void gen_otp(char *hex_key) {
    uint64_t counter = 1; // can be incremented or stored in file/db
    
    int otp = generate_hotp(hex_key, counter);
    if (otp >= 0)
        printf("HOTP code: %06d\n", otp);
    return;
}
*/
void generate_totp(const char *hex_key) {
    uint8_t bin_key[32];
    if (hex_to_bytes(hex_key, bin_key, 32) != 32) {
        fprintf(stderr, "Invalid hex key\n");
        return;
    }
    time_t current_time = time(NULL);
    uint64_t time_step = 60;
    uint64_t counter = current_time / time_step;
    generate_hotp(bin_key, counter);
    return;
}


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// QR CODE
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void    generate_qr(char *hex_key) {
    uint8_t bin_key[32];
    if (hex_to_bytes(hex_key, bin_key, 32) != 32) {
        fprintf(stderr, "Invalid hex key\n");
        return;
    }
    char seed32[65] = base32_encode(bin_key);
    const char *uri = "otpauth://totp/user@service?secret=<BASE32_SECRET>&issuer=<ISSUER>";
    visualize_qr(uri);
}

void visualize_qr(const char *uri) {
    QRcode *qrcode = QRcode_encodeString(uri, 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!qrcode) {
        fprintf(stderr, "Failed to generate QR code.\n");
        return;
    }

    for (int y = 0; y < qrcode->width; y++) {
        for (int x = 0; x < qrcode->width; x++) {
            unsigned char pixel = qrcode->data[y * qrcode->width + x];
            printf(pixel & 1 ? "██" : "  "); // Use full blocks for visual
        }
        printf("\n");
    }
    QRcode_free(qrcode);
}


char *base32_encode(const uint8_t *bin_key) {
    const char *base32_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    int buffer = 0;
    int bits_left = 0;
    size_t index = 0;

    for (size_t i = 0; i < 32; ++i) {
        buffer <<= 8;
        buffer |= bin_key[i] & 0xFF;
        bits_left += 8;

        while (bits_left >= 5) {
            output[index++] = base32_alphabet[(buffer >> (bits_left - 5)) & 0x1F];
            bits_left -= 5;
        }
    }

    if (bits_left > 0) {
        buffer <<= (5 - bits_left);
        output[index++] = base32_alphabet[buffer & 0x1F];
    }

    output[index] = '\0';
    return output;
}
