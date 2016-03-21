#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int *bit;
    int length;
} BinaryIndexedTree;

void BinaryIndexedTree_addValue(BinaryIndexedTree *tree, int i, int value)
{
    int ix = i;
    while (ix < tree->length)
    {
        tree->bit[ix] += value;
        ix = ix + (ix&-ix);
    }
}

BinaryIndexedTree *BinaryIndexedTree_create(int *array, int length)
{
    BinaryIndexedTree *tree = (BinaryIndexedTree *)malloc(sizeof(BinaryIndexedTree));
    tree->bit = (int *)calloc(length, sizeof(int));
    tree->length = length;
    for (int i = 1; i < length; i++)
        BinaryIndexedTree_addValue(tree, i, array[i]);
    return tree;
}

void BinaryIndexedTree_destroy(BinaryIndexedTree *tree)
{
    free(tree->bit);
    free(tree);
}

int BinaryIndexedTree_getSum(BinaryIndexedTree *tree, int i)
{
    int ix = i;
    int sum = 0;
    while (ix > 0)
    {
        sum += tree->bit[ix];
        ix = ix - (ix&-ix);
    }
    return sum;
}

int BinaryIndexedTree_getSumRange(BinaryIndexedTree *tree, int i, int j)
{
    i--;
    return BinaryIndexedTree_getSum(tree, j) - BinaryIndexedTree_getSum(tree, i);
}

int BinaryIndexedTree_get(BinaryIndexedTree *tree, int i)
{
    return BinaryIndexedTree_getSum(tree, i) - BinaryIndexedTree_getSum(tree, i - 1);
}

int binarySearch(BinaryIndexedTree *tree, int start, int end)
{
    if (start == end)
    {
        if (BinaryIndexedTree_get(tree, start))
            return start;
        return -1;
    }

    int m = (end + start) / 2;
    if (BinaryIndexedTree_getSumRange(tree, start, m))
        return binarySearch(tree, start, m);
    return binarySearch(tree, m + 1, end);
}

int searchParking(BinaryIndexedTree *tree, int i)
{
    if (BinaryIndexedTree_getSumRange(tree, i, tree->length - 1))
        return binarySearch(tree, i, tree->length - 1);
    return binarySearch(tree, 1, i);
}

int main(int argc, char *argv[])
{
    int n;
    int m;
    scanf("%d %d", &n, &m);
    n++;
    int *unused = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++)
        unused[i] = 1;
    BinaryIndexedTree *tree = BinaryIndexedTree_create(unused, n);
    for (int i = 0; i < m; i++)
    {
        char c;
        int k;
        scanf(" %c %d", &c, &k);
        if (c == '-')
        {
            if (!BinaryIndexedTree_get(tree, k))
            {
                BinaryIndexedTree_addValue(tree, k, 1);
                printf("%d\n", 0);
            }
            else
            {
                printf("%d\n", -2);
            }
        }
        else
        {
            int parking = searchParking(tree, k);
            if (parking != -1)
                BinaryIndexedTree_addValue(tree, parking, -1);
            printf("%d\n", parking);
        }
    }
}
