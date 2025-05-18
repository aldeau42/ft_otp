#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <math.h>
#include <time.h>
#include <qrencode.h>

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY_SIZE                32  // AES-256 = 32 bytes
#define IV_SIZE                 16  // CBC mode requires 16-byte IV
#define AES_BLOCK_SIZE          16  // 16 bytes = 128 bits
#define BLOCK_SIZE              16

#define KEY_FILE                ft_otp.key 
#define KEY_LENGTH              64
#define PW_LENGTH               6
#define INTERVAL_TIME           60
#define MAX_UNSUCCESS_ATTEMPT   3

typedef struct {
    unsigned char aes_key[32];
    // char    pw[PW_LENGTH];
    // unsigned int    counter;
    // HMAC-SHA1
    // dynamic binary code
} key;


// OTP
void generate_totp(const char *hex_key);
void generate_hotp(uint8_t *bin_key, uint64_t counter);
void generate_qr(char *hex_key);

// AES
int aes_encrypt(const unsigned char *plaintext, int plaintext_len,
    const unsigned char *aes_key, unsigned char *iv,
    unsigned char *ciphertext);
int aes_decrypt(const unsigned char *ciphertext, int ciphertext_len,
    const unsigned char *aes_key, unsigned char *iv,
    unsigned char *plaintext);    
void create_encrypted_keyfile(const char *hex_key, const unsigned char *aes_key);
char *read_decrypted_keyfile(const unsigned char *aes_key);

// TOOLS
const char	*get_file_extension(const char *url);
bool is_hex(char *key);
bool is_valid_key(char *key);
void create_keyfile();

// OPTIONS
void    get_opt(int ac, char **av);
void    g_option(char *str);
void    k_option(char *str);
