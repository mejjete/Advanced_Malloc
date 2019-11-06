#include "out.h"

double pow_(double num, double power)
{
    double res = 1;
    for(double i; i < power; i++)
        res *= num;
    return res;
}

void wprint(const char *format_string, ...)
{
    if(format_string == NULL && strlen(format_string) > 150)
        return;
    struct buf
    {
        char buffer[150];
        unsigned pos;
    }buff;
    buff.pos = 0;
    char sidebuf[50];
    int st_len = strlen(format_string);
    va_list list;
    va_start(list, format_string);
    for(int i = 0; i < st_len; i++)
    {
        if(format_string[i] == '%')
        {
            char ch = format_string[i + 1];
            if(ch == 'd' || ch == 'D')
            {
                int a = va_arg(list, int);
                char *out_string = itoa(a, sidebuf, 10);
                for(int i = 0; i < strlen(out_string); i++)
                    buff.buffer[buff.pos++] = out_string[i];
                i++;
            }
            /*else if(ch == 'p' || ch == 'P')
            {
                
            }*/
            else if(ch == '\n')
                buff.buffer[buff.pos++] = '\n';
        }
        else 
            buff.buffer[buff.pos++] = format_string[i];
    }
    write(1, buff.buffer, strlen(buff.buffer));
}

void swap(char *ch_1, char *ch_2)
{
    char temp;
    temp = *ch_1;
    *ch_1 = *ch_2;
    *ch_2 = temp;
}

void reverse(char str[], int length) 
{ 
    int start = 0; 
    int end = length -1; 
    while (start < end) 
    { 
        swap(str+start, str+end); 
        start++; 
        end--; 
    } 
} 
// Implementation of itoa() 
char* itoa(int num, char* str, int base) 
{ 
    int i = 0; 
    bool isNegative = false; 
    if (num == 0) 
    { 
        str[i++] = '0'; 
        str[i] = '\0'; 
        return str; 
    } 
    if (num < 0 && base == 10) 
    { 
        isNegative = true; 
        num = -num; 
    } 
    while (num != 0) 
    { 
        int rem = num % base; 
        str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
        num = num/base; 
    }  
    if (isNegative) 
        str[i++] = '-'; 
    str[i] = '\0';
    reverse(str, i); 
    return str; 
} 

void *base;
void *next;

int main() 
{ 
    base = sbrk(0);
    wprint("Hello world %d\n", 45);
    next = sbrk(0); 
    return 0; 
} 

void decToHexa(char *string, char *base) 
{    
    int n = atoi(string);
    char hexaDeciNum[100];  
    int i = 0; 
    while(n!=0) 
    {    
        int temp  = 0;  
        temp = n % 16; 
        if(temp < 10) 
        { 
            base[i] = temp + 48; 
            i++; 
        } 
        else
        { 
            base[i] = temp + 55; 
            i++; 
        } 
        n = n/16; 
    } 
    strcpy(base, hexaDeciNum);
}


