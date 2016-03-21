#include <stdio.h>
#include <stdlib.h>



typedef struct Pair
{
    int fst;
    int snd;
} Pair;

typedef Pair T;

typedef struct QNode QNode;
struct QNode 
{
    T data;
    QNode *next;
};

typedef struct Queue
{
    QNode *head;
} Queue;

typedef struct Node
{
    int *transitions;
    int isTerminal;
} Node;

typedef struct Automata
{
    Node **nodes;
    int count;
    int alphabetLength;
} Automata;

Queue *Queue_create()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->head = NULL;
    return q;
}

void Queue_enqueue(Queue *queue, T value)
{
    QNode *head = queue->head;
    queue->head = (QNode *)malloc(sizeof(QNode));
    queue->head->data = value;
    queue->head->next = head;
}

T Queue_dequeue(Queue *queue)
{
    if (queue->head != NULL)
    {
        T data = queue->head->data;
        queue->head = queue->head->next;
        return data;
    }
    T t = { 0, 0 };
    return t;
}

T Queue_peek(Queue *queue)
{
    if (queue->head != NULL)
        return queue->head->data;
    T t = { 0, 0 };
    return t;
}

int Queue_isEmpty(Queue *queue)
{
    return queue->head == NULL;
}

void Queue_destroy(Queue *queue)
{
    free(queue->head);
    free(queue);
}

Automata *Automata_create(int nodesCount, int alphabetLength, int **transitions, int *terminal)
{
    Automata *a = (Automata *)malloc(sizeof(Automata));
    a->nodes = (Node **)malloc(sizeof(Node) * nodesCount);
    for (int i = 0; i < nodesCount; i++)
    {
        a->nodes[i] = (Node *)malloc(sizeof(Node));
        a->nodes[i]->isTerminal = terminal[i];
        a->nodes[i]->transitions = transitions[i];
    }
    a->count = nodesCount;
    a->alphabetLength = alphabetLength;
    return a;
}

void Automata_destroy(Automata *a)
{
    for (int i = 0; i < a->count; i++)
    {
        free(a->nodes[i]->transitions);
        free(a->nodes[i]);
    }
    free(a->nodes);
    free(a);
}

int areEquivalent(Automata *a1, Automata *a2)
{
    if (a1->alphabetLength != a2->alphabetLength)
        return 0;

    T p = { 0, 0 };
    Queue *queue = Queue_create();
    Queue_enqueue(queue, p);

    if (a1->nodes[p.fst]->isTerminal != a2->nodes[p.snd]->isTerminal)
        return 0;

    int *used1 = (int *)calloc(a1->count, sizeof(int));
    int *used2 = (int *)calloc(a2->count, sizeof(int));
    used1[p.fst] = 1;
    used2[p.snd] = 1;

    while (!Queue_isEmpty(queue))
    {
        T vertices = Queue_dequeue(queue);
        
        int u = vertices.fst;
        int v = vertices.snd;

        for (int i = 0; i < a1->alphabetLength; i++)
        {
            int next1 = a1->nodes[u]->transitions[i];
            int next2 = a2->nodes[v]->transitions[i];
            
            if (a1->nodes[next1]->isTerminal != a2->nodes[next2]->isTerminal)
                return 0;

            if (!used1[next1] || !used2[next2])
            {
                T q = { next1, next2 };
                Queue_enqueue(queue, q);
                used1[next1] = 1;
                used2[next2] = 1;
            }
        }
    }
    return 1;
}

int main(int argc, char *argv[])
{
    FILE *in = NULL;
    FILE *out = NULL;
    if (argc > 1)
    {
        in = fopen(argv[1], "r");
        out = fopen(argv[2], "w+");
    }

    Automata **automatas = (Automata **)malloc(sizeof(Automata *) * 2);
    for (int _ = 0; _ < 2; _++)
    {
        int n, k, l;
        in == NULL 
            ? scanf("%d %d %d", &n, &k, &l) 
            : fscanf(in, "%d %d %d", &n, &k, &l);;

        int *terminal = (int *)calloc(n, sizeof(int));
        for (int i = 0; i < k; i++)
        {
            int t;
            in == NULL 
                ? scanf("%d", &t) 
                : fscanf(in, "%d", &t);
            terminal[t] = 1;
        }

        int **transitions = (int **)malloc(sizeof(int *) * n);
        for (int i = 0; i < n; i++)
            transitions[i] = (int *)malloc(sizeof(int) * l);
        for (int i = 0; i < n * l; i++)
        {
            int from, to;
            char c;
            in == NULL 
                ? scanf("%d %c %d", &from, &c, &to) 
                : fscanf(in, "%d %c %d", &from, &c, &to);
            transitions[from][c - 'a'] = to;
        }

        automatas[_] = Automata_create(n, l, transitions, terminal);
    }
    
    int equivalent = areEquivalent(automatas[0], automatas[1]);

    out == NULL 
        ? printf("%s", equivalent ? "EQUIVALENT" : "NOT EQUIVALENT")
        : fprintf(out, "%s", equivalent ? "EQUIVALENT" : "NOT EQUIVALENT");
    
    for (int _ = 0; _ < 2; _++)
        Automata_destroy(automatas[_]);
    
    free(automatas);
}
