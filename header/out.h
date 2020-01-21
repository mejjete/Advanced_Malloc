#include <stdio.h>
#include <stdlib.h>
#ifdef __linux__
    #include <unistd.h>
#endif
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <fcntl.h>

void wprint(const char *format_string, ...);
char* itoa(int num, char* str, int base);
void reverse(char str[], int length);
void swap(char *ch_1, char *ch_2);
void decToHexa(char *string, char *base);
double pow_(double num, double power);
