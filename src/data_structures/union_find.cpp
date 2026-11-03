/**
 * Uso: UnionFind UF(N); UF.unionSet(i, j); UF.isSameSet(i, j); UF.findSet(i); UF.sizeOfSet(i); UF.numDisjointSets()
 * Complejidad: O(alpha(n)) amortizado por operacion (compresion de caminos + union por rango)
 * Fuente: Competitive Programming 4 (segun estilo del codigo original)
 * Status: untested
 */
class UnionFind
{
private:
    vi p, rank, setSize;
    // OOP style
    // vi p is the key part
    int numSets;

public:
    UnionFind(int N)
    {
        p.assign(N, 0);
        for (int i = 0; i < N; ++i)
            p[i] = i;
        rank.assign(N, 0);
        // optional speedup
        setSize.assign(N, 1);
        numSets = N;
    }
    // optional feature
    // optional feature
    int findSet(int i) { return (p[i] == i) ? i : (p[i] = findSet(p[i])); }
    bool isSameSet(int i, int j) { return findSet(i) == findSet(j); }
    int numDisjointSets() { return numSets; }
    // optional
    int sizeOfSet(int i) { return setSize[findSet(i)]; } // optional
    void unionSet(int i, int j)
    {
        if (isSameSet(i, j))
            return;
        int x = findSet(i), y = findSet(j);
        if (rank[x] > rank[y])
            swap(x, y);
        p[x] = y;
        if (rank[x] == rank[y])
            ++rank[y];
        setSize[y] += setSize[x];
        --numSets;
    }
};
