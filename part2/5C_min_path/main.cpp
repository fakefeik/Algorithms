#include <iostream>
#include <vector>
#include <deque>

#define INT_MAX 2147483647

using namespace std;

int bfs(vector<vector<pair<int, int>>> g, int n, int a, int b)
{
    deque<int> q;
    q.push_back(a);
    vector<int> d(n);
    
    for (int i = 0; i < n; i++)
        d[i] = INT_MAX;

    d[a] = 0;
    while (!q.empty())
    {
        int v = q.front();
        q.pop_front();
        for (int i = 0; i < g[v].size(); i++)
        {
            int to = g[v][i].first;
            int w = g[v][i].second;
            if (d[to] > d[v] + w)
            {
                d[to] = d[v] + w;
                w == 0 ? q.push_front(to) : q.push_back(to);
            }
        }
    }
    return d[b] == INT_MAX ? -1 : d[b];
}

int main(int argc, char *argv[])
{
    int n, m, a, b;
    cin >> n >> m >> a >> b;
    vector<vector<pair<int, int>>> g(n);
    for (int i = 0; i < n; i++)
        g[i] = vector<pair<int, int>>();

    for (int i = 0; i < m; i++)
    {
        
        int from, to, weight;
        cin >> from >> to >> weight;
        g[from - 1].push_back(pair<int, int>(to - 1, weight));
        g[to - 1].push_back(pair<int, int>(from - 1, weight));
    }
    cout << bfs(g, n, a - 1, b - 1) << endl;
}
