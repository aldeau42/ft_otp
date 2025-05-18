#include "ft_otp.h"

////////////////////////////////////////////////////////////////
// OPTIONS //
////////////////////////////////////////////////////////////////


// -gen key.hex file option
void g_option(char *str, key *key) {
    char *hex64key;
    FILE *file = fopen(str, "r");
    
    if (!file) {
        if (is_valid_key(str) == false) {
            printf("Error: The key you provided is not valid. It must contain 64 caracters and be hexadecimal.\n");
            return;
        }
        hex64key = str;
    }
    else {
        char buffer[64];
        size_t read = fread(buffer, 1, sizeof(buffer), file);

        if (read != sizeof(buffer)) {
            fprintf(stderr, "Could not read full data\n");
            return;
        } else {
            buffer[read] = '\0';
            if (is_valid_key(buffer) == false)
                return;                    
            hex64key = buffer;
            fclose(file);
        }
    }
    create_keyfile(hex64key, key);
}

// -path option
void k_q_option(char *str, char opt, key *key) {
    char *hex64key;
    if (strcmp(str, "ft_otp.key") != 0) {
        perror("Error: No 64 character hexadecimal key available");
    }

    FILE *file = fopen(str, "r");
    if (!file) {
        perror("Error: No 64 character hexadecimal key available");
    } else {
        char buffer[64];
        size_t read = fread(buffer, 1, sizeof(buffer), file);

        if (read != sizeof(buffer)) {
            fprintf(stderr, "Could not read full data\n");
            return;
        }
        else {
            buffer[read] = '\0';
            if (is_valid_key(buffer) == false) {
                printf("The key you provided is not valid. It must contain 64 caracters and be hexadecimal.\n");
                return;
            }
            hex64key = buffer;
            fclose(file);
        }
    }
    if (opt == 'k')
        generate_totp(hex64key, key);
    else
    generate_qr(hex64key, key);
}

void    get_opt(int ac, char **av) {
    for (int i = 1; i < ac; i++) {
        key *key;
        if (strcmp(av[i], "-g") == 0)
            g_option(av[i + 1], key);
        else if (strcmp(av[i], "-k") == 0)
            k_q_option(av[i + 1], 'k', key);
        else if (strcmp(av[i], "-qr") == 0)
            k_q_option(av[i + 1], 'q', key);
    }
}


//////////////////////////////////////////////////////////////
// MAIN //
//////////////////////////////////////////////////////////////

int main(int ac, char **av) {
    if (ac != 3) {
        fprintf(stderr, "Usage: ./ft_otp [-g <filename>] or [-k ft_otp.key]\n"); 
        return 1;
    }
    get_opt(ac, av);
    return 0;
}
