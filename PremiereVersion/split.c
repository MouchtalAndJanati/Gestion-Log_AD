#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <ctype.h>
#define NUL '\0'

FILE * ouvrirUnFichier(char * nomFichier)
{
    FILE * f = fopen(nomFichier , "r");
    
    return f;
}

char * lireLigneDuFichier(FILE * f)
{
    char * line = NULL;
    if ( f != NULL)
    {
        line = malloc(500 * sizeof(char));
        fgets ( line, 500 * sizeof(char), f );
    }
    return line;  
}

char *trim(char *str)
{
    char *ibuf = str, *obuf = str;
    int i = 0, cnt = 0;
       
    if (str)
    {
        
        for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf)
            ;
        if (str != ibuf)
            memmove(str, ibuf, ibuf - str);
        
        while (*ibuf)
        {
            if (isspace(*ibuf) && cnt)
                ibuf++;
            else
            {
                if (!isspace(*ibuf))
                    cnt = 0;
                else
                {
                    *ibuf = ' ';
                    cnt = 1;
                }
                obuf[i++] = *ibuf++;
            }
        }
        obuf[i] = NUL;
        
       
        while (--i >= 0)
        {
            if (!isspace(obuf[i]))
                break;
        }
        obuf[++i] = NUL;
    }
    return str;
}

char * split(FILE * f)
{
    while (1==1) 
    {
       char * str = lireLigneDuFichier(f);
       char* token = strtok(str, ":");
    
       token = strtok(NULL, ":");
    
       token = trim(token);
  
       if(token != NULL)
       return token;
    }
}

int main()
{
    
    FILE * f = ouvrirUnFichier("test.txt");
    split(lireLigneDuFichier(f));
    
    return 0;
}
