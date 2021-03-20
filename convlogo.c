#include <stdio.h>
#include <fcntl.h>

#define BMSTRUCTSZ 16
#define BUFSZ 4096

char buf[BUFSZ];
char * psz;
int line;

int main(int argc, char ** argv);
void dolines();
int widen(char c);

int main(int argc, char ** argv)
{
    int FileHandle, iMax, i, j;

    // Open and seek past bitmap struct.
    if ((FileHandle = open(argv[1], O_RDONLY | O_BINARY)) == (-1))
    {
        fprintf(stderr, "open failed\n");
        fprintf(stderr, "usage:  convlogo logo.bmp\n");
        exit(-1);
    }
    if (lseek(FileHandle, (long) BMSTRUCTSZ, 0) == -1L)
    {
        fprintf(stderr, "seek failed\n");
        exit(-1);
    }

    // read data into buffer
    if ((iMax = read(FileHandle, buf, BUFSZ)) == -1)
    {
        fprintf(stderr, "read failed\n");
        exit(-1);
    }

    // convert the buffer
    for (i = 0; i < iMax; i++)
        buf[i] ^= 0xff;

    // print it out neatly
    printf("        db      \"Microsoft Windows\",0           ;Product description line 1\n");
    printf("        db      \"Version 1.02\",0                ;Product description line 2\n\n");
    printf("        db      0                               ;Copyright line 1\n");
    printf("        db      0                               ;Copyright line 2\n");
    printf("        db      0                               ;Copyright line 3\n");
    printf("        db      0                               ;Copyright line 4\n");
    printf("                                                ;Copyright line 5\n");
    printf("        db      \"Copyright (c) Microsoft Corporation, 1985, 1986.\"\n");
    printf("        db          \"  All Rights Reserved.\",0\n");
    printf("                                                ;Copyright line 6\n");
    printf("        db      \"Microsoft is a registered trademark of Microsoft Corp.\",0\n\n");
    printf("        db      1                               ;Merging logo\n");
    printf("        db      36                              ;Height of logo (should be even)\n");
    printf("        db      67                              ;Width  of logo\n");
    printf("                                                ;Scan line data.  Even lines\n");
    printf("                                                ; first, followed by odd lines.");

    // even scan lines
    psz = buf;
    line = 0;
    while (psz < & buf[iMax])
    {
        dolines(line);
        psz += 34; // skip the odd scan lines for now
        line += 2;
    }

    // odd scan lines
    line = 1;
    psz = buf + 34;
    while (psz < & buf[iMax])
    {
        dolines(line);
        psz += 34; // skip the even scan lines this time
        line += 2;
    }

    exit(0);
}

void dolines(int line)
{
    int i, j, iwide;
    char c;
    // six full lines
    for (i = 0; i < 6; i++)
    {
        iwide = widen( * psz++);
        printf("\n        db      %.3XH", ((iwide >> 8) & 0xff));
        printf(",%.3XH", (iwide & 0xff));
        for (j = 0; j < 4; j++)
        {
            iwide = widen( * psz++);
            printf(",%.3XH", ((iwide >> 8) & 0xff));
            printf(",%.3XH", (iwide & 0xff));
        }
        if (i == 0)
        {
            printf(" ; line %d", line);
        }
    }
    // then a partial line
    iwide = widen( * psz++);
    printf("\n        db      %.3XH", ((iwide >> 8) & 0xff));
    printf(",%.3XH", (iwide & 0xff));
    for (j = 0; j < 2; j++)
    {
        iwide = widen( * psz++);
        printf(",%.3XH", ((iwide >> 8) & 0xff));
        printf(",%.3XH", (iwide & 0xff));
    }
    iwide = widen( * psz++);
    printf(",%.3XH", (iwide & 0xff));
}

int widen(char c)
{
    int i, j, result;
    char d;

    result = 0;
    j = 1; // j = i ** 2
    // loop through the bits of c
    for (i = 1; i < 9; i++)
    {
        d = c & 0x01;
        c >>= 1;
        result += d * j * j;
        j *= 2;
    }
    result += result * 2; // fill in the empties.
    return (result);
}
