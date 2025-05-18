#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <openssl/hmac.h>
#include <time.h>
#include <qrencode.h>

#define KEY_FILE       "ft_otp.key" 
#define MAX_KEY_LENGTH 1024
#define MIN_KEY_LENGTH 64
#define INTERVAL_TIME  30

#define KEY_SIZE                32  // AES-256 = 32 bytes
#define IV_SIZE                 16  // CBC mode requires 16-byte IV
#define AES_BLOCK_SIZE          16  // 16 bytes = 128 bits
#define BLOCK_SIZE              16

typedef struct {
    unsigned char aes_key[KEY_SIZE];
} ft_key_t;

void generate_totp(void);
void generate_qr(void);
void visualize_qr(const char *uri);
char *base32_encode(const uint8_t *bin_key);
bool is_valid_key(char *key);
bool is_hex(char *key);
bool create_keyfile(const char *hex_key);
int hex_to_bytes(const char *hex, unsigned char *out, int max_len);
void g_option(const char *str);
void k_q_option(char opt);
void get_opt(int ac, char **av);