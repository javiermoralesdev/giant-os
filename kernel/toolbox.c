#include "toolbox.h"
#include "memory.h"
#include "types.h"

int my_strlen(const char *s){
    int length = 0;
    for (length = 0; s[length] != '\0'; length++);
    return length;
    
}

char* my_strcat(const char *s1, const char* s2){
    char* data = kmalloc(my_strlen(s1) + my_strlen(s2) + 1);
    if (data == NULL) {
        panik("HEAP IS FULL");
    }
    int ptr = 0;
    for (int i = 0; s1[i] != '\0'; i++) {
        data[ptr++] = s1[i];
    }
    for (int i = 0; s2[i] != '\0'; i++) {
        data[ptr++] = s2[i];
    }
    return data;
}