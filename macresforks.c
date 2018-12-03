#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>

/*
#############################################################################

MacResForks

Author: Michael Lilley (michael.lilley@gmail.com)
Date: 2018/12/03
License: MIT

Description:
A filter to assist with the removal of mac resource forks from non-mac
filesystems, by filtering a piped list of filenames for only those that look
like mac resource fork files AND actually have a corresponding regular file.

Usage:
$ find / -name '._*' -print0 | macresforks | xargs -0 -r rm

Build:
$ make

Install:
$ sudo cp macresforks /usr/local/bin   # ensure it's in your path

#############################################################################
*/

const char *VERSION = "0.0.4";

// Reads a null terminated string from f, and returns it.
//  - returns non-zero pointer to string on success, or EOF
//  - returned string should be free()d by caller.
char *nextStr(FILE *f)
{
    size_t len = 16;
    size_t n = 0;

    char *str = malloc(sizeof(char) * len);
    if (!str)
    {
        fputs("out of memory", stderr);
        exit(1);
    }

    int ch;
    while (1)
    {
        ch = fgetc(f);

        // if we hit EOF, return it only if we haven't read anything yet,
        // otherwise return what we have read (EOF will be returned in the
        // next call to nextStr)...
        if (ch == EOF)
        {
            if (n == 0)
            {
                free(str);
                return 0;
            }
            break;
        }

        // if we read a null, stop reading and return what we've got
        if (ch == '\0')
            break;

        // copy char into buffer
        str[n++] = ch;

        // if we've run out of space, double the buffer size
        if (n == len)
        {
            len = len * 2;
            char *tmp = realloc(str, sizeof(char) * len);
            if (!tmp)
            {
                fputs("out of memory", stderr);
                exit(1);
            }
            str = tmp;
        }
    }

    // terminate the string with null
    str[n++] = '\0';

    // resize buffer to exact size of string
    if (n != len)
    {
        char *tmp = realloc(str, sizeof(char) * n);
        if (!tmp)
        {
            fputs("out of memory", stderr);
            exit(1);
        }
        str = tmp;
    }

    return str;
}

// Returns 1 if str is a resfork and a corresponding non-resfork file exists
int isVerifiedResFork(char *str)
{
    // NB: - basename() and dirname() can modify their input, so pass copy
    //     - basename() and dirname() result should not be free()ed
    char *tmp = malloc(sizeof(char) * (strlen(str) + 1));
    if (!tmp)
    {
        fputs("out of memory", stderr);
        exit(1);
    }

    // ensure the name looks like a resfork
    char *base = basename(strcpy(tmp, str));
    size_t nbase = strlen(base);
    if (nbase < 2 || base[0] != '.' || base[1] != '_')
    {
        free(tmp);
        return 0;
    }

    // construct the full non-resfork name
    char *dir = dirname(strcpy(tmp, str));
    size_t ndir = strlen(dir);
    strcpy(tmp, dir);
    tmp[ndir] = '/';
    for (size_t x = 0; x < nbase - 2 + 1; x++)
    {
        tmp[ndir + 1 + x] = base[x + 2];
    }

    // check the non-resfork file exists
    int exists = (access(tmp, F_OK) == 0);
    free(tmp);
    return exists;
}

void help()
{
    puts("usage: find . -name '._*' -print0 | macresforks | xargs -r -0 rm");
    puts("");
    puts("  --help     display help text");
    puts("  --version  display version details");
}

void version()
{
    printf("macresforks %s\n", VERSION);
}

int main(int argc, char *argv[])
{
    if (argc >= 2)
    {
        if (strcmp(argv[1], "--help") == 0)
        {
            help();
        }
        else if (strcmp(argv[1], "--version") == 0)
        {
            version();
        }
        return 0;
    }

    // for all strings in input, copy to output if verified as a resfork
    char *str;
    while ((str = nextStr(stdin)))
    {
        if (isVerifiedResFork(str))
        {
            fprintf(stdout, "%s", str);
            fprintf(stdout, "%c", '\0');
        }
        free(str);
    }
}
