#include <cstdio>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <set>
#include <string>

#define MAX_LEN 100000
#define ALPH_SIZE 123

int *build_suffix_array(char *s, int n)
{
    int *pos = new int[n];
    int *rank = new int[n];
    int *cont = new int[std::max(ALPH_SIZE, n)];
    int *next = new int[n];
    bool *bh = new bool[n + 1];
    bool *b2h = new bool[n + 1];

    memset(cont, 0, sizeof(int) * std::max(ALPH_SIZE, n));

    for (int i = 0; i < n; i++)
        cont[s[i]]++;
    for (int i = 1; i < ALPH_SIZE; i++)
        cont[i] += cont[i - 1];
    for (int i = 0; i < n; i++)
        pos[--cont[s[i]]] = i;

    for (int i = 0; i < n; i++)
    {
        bh[i] = i == 0 || s[pos[i]] != s[pos[i - 1]];
        b2h[i] = false;
    }

    for (int h = 1; h < n; h <<= 1)
    {
        int buckets = 0;

        for (int i = 0, j; i < n; i = j)
        {
            j = i + 1;

            while (j < n && !bh[j]) ++j;
            next[i] = j;
            buckets++;
        }

        if (buckets == n)
            break;

        for (int i = 0; i < n; i = next[i])
        {
            cont[i] = 0;
            for (int j = i; j < next[i]; j++)
                rank[pos[j]] = i;
        }

        cont[rank[n - h]]++;
        b2h[rank[n - h]] = true;

        for (int i = 0; i < n; i = next[i])
        {
            for (int j = i; j < next[i]; j++)
            {
                int p = pos[j] - h;
                if (p >= 0)
                {
                    int head = rank[p];
                    rank[p] = head + cont[head];
                    cont[head]++;
                    b2h[rank[p]] = true;
                }
            }

            for (int j = i; j < next[i]; j++)
            {
                int p = pos[j] - h;
                if (p >= 0 && b2h[rank[p]])
                {
                    for (int k = rank[p] + 1; !bh[k] && b2h[k]; k++)
                        b2h[k] = false;
                }
            }
        }

        for (int i = 0; i < n; i++)
        {
            pos[rank[i]] = i;
            bh[i] |= b2h[i];
        }
    }

    delete[] rank;
    delete[] cont;
    delete[] next;
    delete[] bh;
    delete[] b2h;

    return pos;
}

int *lcp(char *s, int *sa, int n)
{
    int *rank = new int[n];
    for (int i = 0; i < n; i++)
        rank[sa[i]] = i;
    
    int *lcp = new int[n - 1];
    for (int i = 0, h = 0; i < n; i++)
        if (rank[i] < n - 1)
        {
            int j = sa[rank[i] + 1];
            while (std::max(i, j) + h < n && s[i + h] == s[j + h])
                h++;
            lcp[rank[i]] = h;
            if (h > 0)
                h--;
        }

    delete[] rank;
    return lcp;
}

typedef struct
{
    int *logTable;
    int **rmq;
    int *a;
    int n;
} Rmq;

Rmq *rmq_new(int *a, int n)
{
    Rmq *rmq = new Rmq;
    rmq->a = new int[n];
    for (int i = 0; i < n; i++)
        rmq->a[i] = a[i];
    rmq->logTable = new int[n + 1];
    rmq->n = n;
    for (int i = 0; i < n + 1; i++)
        rmq->logTable[i] = 0;

    for (int i = 2; i <= n; i++)
        rmq->logTable[i] = rmq->logTable[i >> 1] + 1;

    rmq->rmq = new int*[rmq->logTable[n] + 1];
    for (int i = 0; i < rmq->logTable[n] + 1; i++)
        rmq->rmq[i] = new int[n];

    for (int i = 0; i < n; ++i)
        rmq->rmq[0][i] = i;

    for (int i = 0; i < n; i++)
        for (int j = 1; j < rmq->logTable[n] + 1; j++)
            rmq->rmq[j][i] = 0;

    for (int k = 1; 1 << k < n; ++k)
        for (int i = 0; i + (1 << k) <= n; i++)
        {
            int x = rmq->rmq[k - 1][i];
            int y = rmq->rmq[k - 1][i + (1 << k - 1)];
            rmq->rmq[k][i] = a[x] <= a[y] ? x : y;
        }
    
    return rmq;
}

int rmq_min_pos(Rmq *rmq, int i, int j)
{
    int k = rmq->logTable[j - i];
    int x = rmq->rmq[k][i];
    int y = rmq->rmq[k][j - (1 << k) + 1];
    return rmq->a[x] <= rmq->a[y] ? x : y;
}

void rmq_destroy(Rmq *rmq)
{
    delete[] rmq->a;
    for (int i = 0; i < rmq->logTable[rmq->n] + 1; i++)
        delete[] rmq->rmq[i];
    delete[] rmq->logTable;
    delete[] rmq->rmq;
    delete[] rmq;
}

int main()
{
    char S[MAX_LEN];
    scanf("%s", S);
    int n = strlen(S);
    int *pos = build_suffix_array(S, n);
    
    int *lcpArray = lcp(S, pos, n);

    int *posInverse = new int[n];
    for (int i = 0; i < n; i++)
        posInverse[pos[i]] = i;

    Rmq *rmq = rmq_new(lcpArray, n - 1);

    auto pairs = std::set<std::pair<int, int>>();

    for (int i = 0; i < n; i++)
    {
        pairs.emplace(std::make_pair(posInverse[i], i));
        int res = 0;
        auto it = pairs.lower_bound(std::make_pair(posInverse[i], i));
        if (it != pairs.begin())
            res = std::max(res, lcpArray[rmq_min_pos(rmq, std::prev(it)->first, posInverse[i] - 1)]);
        if (std::next(it) != pairs.end())
            res = std::max(res, lcpArray[rmq_min_pos(rmq, posInverse[i], std::next(it)->first-1)]);
        std::cout << res << std::endl;
    }
    rmq_destroy(rmq);
    delete[] posInverse;
    delete[] lcpArray;
}
