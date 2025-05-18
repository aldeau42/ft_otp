#include "ft_otp.h"

////////////////////////////////////////////////////////////////
// OPTIONS //
////////////////////////////////////////////////////////////////

void g_option(const char *str) {
    FILE *file = fopen(str, "r");
    char buffer[MAX_KEY_LENGTH + 1];

    if (file) {
        size_t read = fread(buffer, 1, MAX_KEY_LENGTH, file);
        buffer[read] = '\0';
        fclose(file);
        create_keyfile(buffer);
    } else if (is_valid_key((char *)str)) {
        create_keyfile(str);
    } else {
        fprintf(stderr, "Error: Provided key is invalid or file not found.\n");
    }
}

void k_q_option(char opt) {
    if (opt == 'k')
        generate_totp();
    else if (opt == 'q')
        generate_qr();
}

void get_opt(int ac, char **av) {
    if (ac < 3) return;
    if (strcmp(av[1], "-g") == 0) {
        g_option(av[2]);
    } else if (strcmp(av[1], "-k") == 0) {
        k_q_option('k');
    } else if (strcmp(av[1], "-qr") == 0) {
        k_q_option('q');
    }
}

//////////////////////////////////////////////////////////////
// MAIN //
//////////////////////////////////////////////////////////////

int main(int ac, char **av) {
    if (ac != 3) {
        fprintf(stderr, "Usage: ./ft_otp [-g <filename>] or [-k <key>] or [-qr <key>]\n"); 
        return 1;
    }
    get_opt(ac, av);
    return 0;
}
