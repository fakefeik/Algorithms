#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size(x) sizeof(x) / sizeof(x[0])
#define BUFFER_SIZE 1024

typedef char T;

typedef struct {
    T *elements;
    size_t size;
    size_t capacity;
} List;

List *newList()
{
    List *list = (List *)malloc(sizeof(List));
    list->size = 0;
    list->capacity = 16;
    list->elements = (T *)malloc(sizeof(T) * list->capacity);
    return list;
}

void deallocList(List *list)
{
    free(list->elements);
    free(list);
}

void addToList(List *list, T element)
{
    if (list->size < list->capacity)
    {
        list->elements[list->size++] = element;
    }
    else
    {
        list->capacity *= 2;
        T *newElements = (T *)malloc(sizeof(T) * list->capacity);
        for (int i = 0; i < list->size; i++)
            newElements[i] = list->elements[i];
        newElements[list->size] = element;
        free(list->elements);
        list->elements = newElements;
        list->size++;
    }
}

T peekFromList(List *list)
{
    if (list->size == 0)
        return 0;
    return list->elements[list->size - 1];
}

T popFromList(List *list)
{
    if (list->size == 0)
        return 0;
    list->size--;
    return list->elements[list->size];
}

int isOpenBracket(char bracket)
{
    return bracket == '(' || bracket == '{' || bracket == '[';
}

char mapOpenBracket(char closeBracket)
{
    switch (closeBracket)
    {
    case '}':
        return '{';
    case ')':
        return '(';
    case ']':
        return '[';
    default:
        return 0;
    }
}

int checkBrackets(const char *brackets)
{
    List *chars = newList();
    int countOpen = 0;
    int countClose = 0;
    size_t len = strlen(brackets);
    for (size_t i = 0; i < len; i++) 
    {
        if (isOpenBracket(brackets[i])) 
        {
            addToList(chars, brackets[i]);
            countOpen++;
        }
        else
        {
            if (chars->size == 0 || popFromList(chars) != mapOpenBracket(brackets[i]))
            {
                deallocList(chars);
                return i;
            }
            countClose++;
        }
    }
    deallocList(chars);
    if (countOpen != countClose) 
        return len;
    return -1;
}

const char *getline()
{
    int accumulatorSize = BUFFER_SIZE;
    char* accumulator = malloc(accumulatorSize);
    char readBuf[BUFFER_SIZE];
    
    *accumulator = '\0';

    while (!feof(stdin))
    {
        fgets(readBuf, BUFFER_SIZE, stdin);
        strcat(accumulator, readBuf);
        if (readBuf[strlen(readBuf) - 1] != '\n')
        {
            accumulatorSize += BUFFER_SIZE;
            accumulator = realloc(accumulator, accumulatorSize);
        }
        else 
        {
            break;
        }
    }
    accumulator[strlen(accumulator) - 1] = '\0';
    return accumulator;
}

int main(int argc, char* argv[])
{
    const char *brackets = getline();
    int result = checkBrackets(brackets);
    if (result == -1)
        printf("CORRECT\n");
    else
        printf("%d\n", result);
}
