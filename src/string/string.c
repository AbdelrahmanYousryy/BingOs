#include "string.h"


/** Calculate the length of string `*ptr` */
int strlen(const char* ptr)
{

    int i = 0;
    while(*ptr != 0)
    {
        i++;
        ptr+=1;
    }
    return i;
}

/** Check if the character `c` is digit */
bool isdigit(char c)
{
    return c >= 48 && c <= 57;
}

/** Convert character `c` to numeric */
int tonumericdigit(char c)
{
    return c - 48;
}

/** Calculate the length of string `*ptr` with maximum length `max` */
int strnlen(const char* ptr , int max)
{
    int i =0 ;
    for(i = 0 ; i < max ; i++)
    {
        if(ptr[i] == 0)break;

    }
    return i ;
}

/** Copy `src` into `dest` */
char* strcpy(char* dest , const char* src)
{
    char* res = dest;
    while(*src != '\0')
    {
        *dest = *src;
         src += 1;
         dest += 1;
    }
    // append the null terminator
    *dest = '\0';
    return res;
}