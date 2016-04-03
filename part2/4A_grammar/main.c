#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct
{
    char *s;
    int len;
} String;

String *String_create(char *s, int len)
{
    String *string = (String *)malloc(sizeof(String));
    string->s = s;
    string->len = len;
    return string;
}

void String_destroy(String *string)
{
    free(string->s);
    free(string);
}

String *getline()
{
    char *line = malloc(100);
    char *linep = line;
    size_t lenmax = 100, len = lenmax;
    int c;

    if (line == NULL)
        return NULL;

    while (true)
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
    return String_create(linep, line - linep - 1);
}

int main()
{
    int n;
    scanf("%d\n", &n);
    String **rules = (String **)malloc(sizeof(String *) * n);
    for (int i = 0; i < n; i++)
        rules[i] = getline();

}
