#include <stdio.h>
#include <stdlib.h>

#define min(a,b) (((a) < (b)) ? (a) : (b))

typedef struct
{
    char *s;
    int len;
} String;

String *getline()
{
    String *string = (String *)malloc(sizeof(String));
    char *line = malloc(100);
    char *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    while (1)
    {
        c = fgetc(stdin);
        if (c == EOF)
            break;

        if (--len == 0)
        {
            len = lenmax;
            char * linen = realloc(linep, lenmax *= 2);

            if (linen == NULL)
            {
                free(linep);
                return NULL;
            }
            line = linen + (line - linep);
            linep = linen;
        }

        if ((*line++ = c) == '\n')
            break;
    }
    *line = '\0';
    string->s = linep;
    string->len = line - linep - 1;
    return string;
}

int *zFunction(String *s)
{
    int n = s->len;
    int *z = (int *)calloc(n, sizeof(int));
    z[0] = s->len;
    int l = 0;
    int r = 0;
    for (int i = 1; i < n; i++)
    {
        if (i <= r)
            z[i] = min(r - i + 1, z[i - l]);
        while (i + z[i] < n && s->s[z[i]] == s->s[i + z[i]])
            z[i]++;
        if (i + z[i] - 1 > r)
        {
            l = i; 
            r = i + z[i] - 1;
        }
    }
    return z;
}

int main()
{
    String *input = getline();
    int *result = zFunction(input);
    for (int i = 0; i < input->len; i++)
        printf("%d ", result[i]);
}
