#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

typedef map<int, vector<vector<int>>> grammar;

int symbolToInt(char c)
{
    if (c == '$')
        return 26;
    if (c == 'S')
        return 27;
    if (c >= 'a' && c <= 'z')
        return c - 'a';
    if (c < 'S')
        return 28 + c - 'A';
    return 27 + c - 'A';
}

string getString(int symbol)
{
    return string(1, 'a' + symbol);
}

bool isNonterminal(int symbol)
{
    return symbol > 26;
}

int getMaxNonterminal(grammar &g)
{
    auto maxSymbol = 0;
    for (auto rule : g)
    {
        maxSymbol = max(maxSymbol, rule.first);
        auto productions = rule.second;
        for (auto production : rule.second)
            for (auto s : production)
                maxSymbol = max(maxSymbol, s);
    }
    return maxSymbol;
}

void createProduction(grammar &g, int nonterminal, int symbol)
{
    if (g.find(nonterminal) == g.end())
        g[nonterminal] = vector<vector<int>>();
    g[nonterminal].push_back(vector<int>(1, symbol));
}

void createProduction(grammar &g, int nonterminal, vector<int> &production)
{
    if (g.find(nonterminal) == g.end())
        g[nonterminal] = vector<vector<int>>();
    g[nonterminal].push_back(production); 
}

grammar decreaseStringSize(grammar &g)
{
    grammar newGrammar;
    auto maxSymbol = getMaxNonterminal(g);
    for (auto rule : g)
    {
        int nonterminal = rule.first;
        newGrammar[nonterminal] = vector<vector<int>>();
        auto &newProductions = newGrammar[nonterminal];
        for (auto production : rule.second)
        {
            if (production.size() == 1)
            {
                newProductions.push_back(production);
                continue;
            }
            
            newProductions.push_back(vector<int>(2));
            auto &line = newProductions.back();
            
            if (isNonterminal(production[0]))
            {
                line[0] = production[0];
            }
            else
            {
                maxSymbol++;
                createProduction(newGrammar, maxSymbol, production[0]);
                line[0] = maxSymbol;
            }
            
            maxSymbol++;
            line[1] = maxSymbol;
            for (int i = 1; i < production.size(); i++)
                if (i == production.size() - 1)
                {
                    createProduction(newGrammar, maxSymbol, production[i]);
                }
                else
                {
                    newGrammar[maxSymbol] = vector<vector<int>>(1);
                    newGrammar[maxSymbol][0] = vector<int>(2);
                    auto &currentProduction = newGrammar[maxSymbol][0];
                    if (isNonterminal(production[i]))
                    {
                        currentProduction[0] = production[i];
                    }
                    else
                    {
                        maxSymbol++;
                        createProduction(newGrammar, maxSymbol, production[i]);
                        currentProduction[0] = maxSymbol;
                    }
                    maxSymbol++;
                    currentProduction[1] = maxSymbol;
                }
        }
    }
    return newGrammar;
}

set<int> getImmediatelyNullable(grammar &g)
{
    auto immediatelyNullable = set<int>();
    for (auto rule : g)
    {
        auto nonterminal = rule.first;
        for (auto production : rule.second)
            if (production.size() == 1 && production[0] == symbolToInt('$'))
                immediatelyNullable.insert(nonterminal);
    }
    return immediatelyNullable;
}

set<int> getNullable(grammar &g)
{
    auto nullable = getImmediatelyNullable(g);
    
    if (nullable.size() == 0)
        return nullable;
    
    while (true)
    {
        auto changed = false;
        for (auto rule : g)
        {
            int nonterminal = rule.first;
            if (nullable.find(nonterminal) != nullable.end())
                continue;
            for (auto production : rule.second)
                if (nullable.find(production[0]) != nullable.end() &&
                        (production.size() == 1 || production.size() == 2 &&
                            nullable.find(production[1]) != nullable.end()))
                {
                    nullable.insert(nonterminal);
                    changed = true;
                }
        }

        if (!changed)
            break;
    }
    
    return nullable;
}

grammar removeEpsilonProductions(grammar &g)
{
    auto nullable = getNullable(g);
    grammar newGrammar;
    if (nullable.find(symbolToInt('S')) != nullable.end())
        createProduction(newGrammar, symbolToInt('S'), symbolToInt('$'));
    for (auto rule : g)
    {
        int nonterminal = rule.first;
        for (auto production : rule.second)
        {
            if (production.size() == 1)
            {
                if (production[0] != symbolToInt('$'))
                    createProduction(newGrammar, nonterminal, production[0]);
            }
            else
            {
                if (nullable.find(production[0]) != nullable.end())
                    createProduction(newGrammar, nonterminal, production[1]);
                if (nullable.find(production[1]) != nullable.end())
                    createProduction(newGrammar, nonterminal, production[0]); 
                createProduction(newGrammar, nonterminal, production);
            }
        }
    }
    return newGrammar;
}

void updateOneSymbolProductions(map<int, set<int>> &productions, int nonterminal, grammar &g)
{
    if (productions.find(nonterminal) != productions.end())
        return;
    productions[nonterminal] = set<int>();
    if (g.find(nonterminal) == g.end())
        return;
    for (auto production : g[nonterminal])
    {
        if (production.size() == 1 && isNonterminal(production[0]))
        {
            updateOneSymbolProductions(productions, production[0], g);
            productions[nonterminal].insert(
                productions[production[0]].begin(),
                productions[production[0]].end()
                );
            productions[nonterminal].insert(production[0]);
        }
    }
    if (productions[nonterminal].find(nonterminal) != productions[nonterminal].end())
        productions[nonterminal].erase(nonterminal);
}

map<int, set<int>> getOneSymbolProductions(grammar &g)
{
    auto productions = map<int, set<int>>();
    for (auto rule : g)
    {
        updateOneSymbolProductions(productions, rule.first, g);
        for (auto production : rule.second)
            for (auto s : production)
                if (isNonterminal(s))
                    updateOneSymbolProductions(productions, s, g);
    }
    return productions;
}

grammar removeOneSymbolProductionWithStartSymbol(grammar &g, map<int, set<int>> &productions)
{
    if (productions.find(symbolToInt('S')) == productions.end())
        return g;
    for (auto production : productions[symbolToInt('S')])
    {
        int nonterminal = production;
        if (g.find(nonterminal) == g.end())
            continue;
        for (auto p : g[nonterminal])
            createProduction(g, symbolToInt('S'), p);
    }
    return g;
}

grammar removeOneSymbolProductions(grammar &g)
{
    grammar newGrammar;
    auto productions = getOneSymbolProductions(g);
    for (auto rule : g)
    {
        int nonterminal = rule.first;
        for (auto production : rule.second)
        {
            if (production.size() == 1 && !isNonterminal(production[0]))
                createProduction(newGrammar, nonterminal, production[0]);
            if (production.size() == 2)
            {
                productions[production[0]].insert(production[0]);
                productions[production[1]].insert(production[1]);
                for (auto it1 : productions[production[0]])
                    for (auto it2 : productions[production[1]])
                    {
                        vector<int> newProduction(2);
                        newProduction[0] = it1;
                        newProduction[1] = it2;
                        createProduction(newGrammar, nonterminal, newProduction);
                    }
                productions[production[0]].erase(production[0]);
                productions[production[1]].erase(production[1]);
            }
        }
    }
    newGrammar = removeOneSymbolProductionWithStartSymbol(newGrammar, productions);
    return newGrammar;
}

grammar toChomskyNormalForm(grammar &g)
{
    g = decreaseStringSize(g);
    g = removeEpsilonProductions(g);
    return removeOneSymbolProductions(g);
}

void updateMinStringsWithLength(grammar &g, map<int, vector<string>> &strings, int nonterminal, int length)
{
    if (strings.find(nonterminal) == strings.end())
        strings[nonterminal] = vector<string>();

    if (g.find(nonterminal) == g.end())
    {
        strings[nonterminal].push_back(string());
        return;
    }

    string minString;
    for (auto production : g[nonterminal])
    {
        if (production.size() == 1)
        {
            if (length == 1 && (minString.empty() || minString > getString(production[0])))
                minString = getString(production[0]);
        }
        else
        {
            for (int i = 1; i < length; i++)
            {
                if (strings.find(production[0]) == strings.end() ||
                        strings.find(production[1]) == strings.end())
                    continue;
                if (strings[production[0]][i - 1].empty() ||
                        strings[production[1]][length - i - 1].empty())
                    continue;
                string minCandidate = strings[production[0]][i - 1] + strings[production[1]][length - i - 1];
                if (minString.empty() || minString > minCandidate)
                    minString = minCandidate;
            }
        }
    }
    strings[nonterminal].push_back(minString);
}

bool acceptsEpsilonString(grammar &g)
{
    if (g.find(symbolToInt('S')) == g.end())
        return false;
    for (auto production : g[symbolToInt('S')])
        if (production.size() == 1 && production[0] == symbolToInt('$'))
            return true;
    return false;
}

string getMinString(grammar &g, int k)
{
    if (g.find(symbolToInt('S')) == g.end())
        return "IMPOSSIBLE";
    
    if (acceptsEpsilonString(g))
        return "$";
    
    auto strings = map<int, vector<string>>();
    string minString;
    for (int i = 1; i <= k; i++)
    {
        for (auto rule : g)
        {
            int nonterminal = rule.first;
            updateMinStringsWithLength(g, strings, nonterminal, i);
        }
        if (!strings[symbolToInt('S')][i - 1].empty() &&
            (minString.empty() || minString > strings[symbolToInt('S')][i - 1]))
            minString = strings[symbolToInt('S')][i - 1];
    }
    
    if (minString.empty())
        return "IMPOSSIBLE";
    
    return minString;
}

int main()
{
    grammar g;
    int n;
    cin >> n;
    
    for (int i = 0; i < n; i++)
    {
        string line;
        cin >> line;

        int nonterminal = symbolToInt(line[0]);
        if (g.find(nonterminal) == g.end())
            g[nonterminal] = vector<vector<int>>();
        g[nonterminal].push_back(vector<int>());

        auto &production = g[nonterminal][g[nonterminal].size() - 1];
        for (int lineIndex = 3; lineIndex < line.size(); lineIndex++)
            production.push_back(symbolToInt(line[lineIndex]));
    }

    int k;
    cin >> k;
    g = toChomskyNormalForm(g);
    cout << getMinString(g, k) << endl;
}
