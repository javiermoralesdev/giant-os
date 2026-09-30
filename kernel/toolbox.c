#include "toolbox.h"
#include "memory.h"
#include "types.h"

size_t my_strlen(const char *s) {
    if (!s) return 0;
    size_t length = 0;
    for (length = 0; s[length] != '\0'; length++);
    return length;
}

char* my_strcat(const char *s1, const char* s2) {
    if (!s1 || !s2) return NULL;
    size_t len1 = my_strlen(s1);
    size_t len2 = my_strlen(s2);
    char* data = kmalloc(len1 + len2 + 1);
    if (data == NULL) {
        panik("HEAP IS FULL");
    }
    size_t ptr = 0;
    for (size_t i = 0; i < len1; i++) {
        data[ptr++] = s1[i];
    }
    for (size_t i = 0; i < len2; i++) {
        data[ptr++] = s2[i];
    }
    data[ptr] = '\0';
    return data;
}

int my_strcmp(const char *str1, const char *str2) {
    if (!str1 || !str2) {
        return (str1 == str2) ? 0 : (str1 ? 1 : -1);
    }
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return (int)(unsigned char)*str1 - (int)(unsigned char)*str2;
}

