#include "revert_string.h"

void RevertString(char *str)
{
    char *p = str;
    int n = 0;
    
    while (*p != '\0')
    {
        n++;
        p++;
    }

    char buff;

    for (int i=0; i<n/2; i++)
    {
        buff = str[n-i-1];
        str[n-i-1] = str[i];
        str[i] = buff;
    }
}

