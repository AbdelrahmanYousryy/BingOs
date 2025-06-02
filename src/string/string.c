#include "string.h"


char tolower(char s1)
{
    if(s1 >=65 && s1 <= 90)
    {
        s1+=32;
    }
    return s1;
}

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

/** read a string `str` until terminator `terminator` or length `max` or a null terminator*/
int strnlen_terminator(const char* str , int max , char terminator)
{
    int i = 0 ;
    for(i = 0 ; i < max ; i++)
    {
        if(str[i]== '\0' || str[i] == terminator)
        {
            break;
        }
    }
    return i;
}
/** Case sensetive Compare string `s1` and string `s2` for `n`characters*/
int strncmp(const char* str1 , const char* str2 , int n)
{
    unsigned char u1 , u2;
    while(n-- > 0)
    {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if(u1!= u2)
        {
            return u1-u2;
        }
        if(u1 == '\0')
        {
            return 0;
        }
    }
    return 0;
}

/** Case insensetive Compare string `s1` and string `s2` */
int istrncmp(const char* s1 , const char* s2 , int n)
{
    unsigned char u1 , u2;
    while(n -- >0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if((u1 != u2) && (tolower(u1)!= tolower(u2)))
        {
            return u1-u2;
        }
        if( u1 == '\0')
        {
            return 0;
        }

    }
    return 0;
}

