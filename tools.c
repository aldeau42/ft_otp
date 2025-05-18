#include "ft_otp.h"

////////////////////////////////////////////////////////////////
// TOOLS //
////////////////////////////////////////////////////////////////


bool is_hex(char *key) {
	size_t i = 0;
    while (key[i] != '\0' 
		&& (('0' <= key[i] && key[i] <= '9')
		|| ('a' <= key[i] && key[i] <= 'f')
		|| ('A' <= key[i] && key[i] <= 'F')))
        	i++;
	if (key[++i] != '\0')
		return false;
    return true;
}

bool is_valid_key(char *key) {
    if (is_hex(key) == false || strlen(key) != 64)
        return false;
    return true;
}

/*
const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(url, '.');  // Find last '.'
    if (!dot || strchr(dot, '/'))  // Ensure it's a valid extension
        return "jpg";

    return dot + 1;
}
*/
