#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct node Node;
struct node
{
    int data;
    Node *left;
    Node *right;
};

Node *createNode(int data)
{
    Node *temp = (Node *)malloc(sizeof(Node));

    temp->data = data;
    temp->left = temp->right = NULL;

    return temp;
}

void deallocNode(Node *node)
{
    if (node == NULL)
        return;
    deallocNode(node->left);
    deallocNode(node->right);
    free(node);
}

Node *constructTreeUtil(int *pre, int *preIndex, int key, int min, int max, int size)
{
    if (*preIndex >= size)
        return NULL;

    Node *root = NULL;

    if (key >= min && key < max)
    {
        root = createNode(key);
        *preIndex = *preIndex + 1;

        if (*preIndex < size)
        {
            root->left = constructTreeUtil(pre, preIndex, pre[*preIndex], min, key, size);
            root->right = constructTreeUtil(pre, preIndex, pre[*preIndex], key, max, size);
        }
    }

    return root;
}

Node *constructTree(int *pre, int size)
{
    int preIndex = 0;
    return constructTreeUtil(pre, &preIndex, pre[0], INT_MIN, INT_MAX, size);
}

void printPostorder(Node *node)
{
    if (node == NULL) 
        return;
    printPostorder(node->left);
    printPostorder(node->right);
    printf("%d ", node->data);
}

void printInorder(Node *node)
{
    if (node == NULL)
        return;
    printInorder(node->left);
    printf("%d ", node->data);
    printInorder(node->right);
}

int main()
{
    int n;
    scanf("%d", &n);
    int *pre = (int *)malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
        scanf("%d", &pre[i]);

    Node *root = constructTree(pre, n);

    printPostorder(root);
    printf("\n");
    printInorder(root);
    printf("\n");
    deallocNode(root);

    return 0;
}
