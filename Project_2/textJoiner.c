#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Error opening file %s\n", argv[1]);
        return 1;
    }

    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        putchar(c);
    }

    fclose(fp);

    FILE *fp2 = fopen(argv[2], "r");
    if (fp2 == NULL)
    {
        printf("Error opening file %s\n", argv[2]);
        return 1;
    }
    printf(" ");

    int x;
    while ((x = fgetc(fp2)) != EOF)
    {
        putchar(x);
    }
    printf("\n");
    fclose(fp2);

    return 0;
}