#include "toolbox.h"

int my_strlen(const char *s){
    int length = 0;
    for (length = 0; s[length] == '\0'; length++);
    return length;
    
}