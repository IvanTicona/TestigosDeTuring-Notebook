/**
 * Usage: d[i][j] = edge weight (INFL if none), d[i][i] = 0
 *        floyd(d, nxt) -> all pairs: INFL unreachable, -INFL if a negative
 *                         cycle lies on some i -> j walk; nxt for paths
 *        fpath(nxt, i, j)  -> vertices of a shortest path (d finite)
 *        closure(adj)  -> reach[i][j] (bitset rows, n <= N)
 *        minimax(d)    -> min over paths of the MAX edge (undirected or
 *                         directed; INFL if unreachable)
 * Complexity: O(n^3); closure O(n^3 / 64)
 * Source: UMSS Floyd (no d[i][i] = 0), UMSS Floyd Warshall Negative
 *         Weights, UMSS Transitive Closure, UPB DijkstraFloyd, old
 *         minimax_all_pairs (unreachable pairs were 0); paths: own
 * Note: loop order k, i, j. Maximin (widest path): swap min / max.
 *       Minimax for one pair or many: MST paths (Kruskal) are optimal too.
 * Status: ok
 */
void floyd(vector<vl> &d, vector<vi> &nxt) {
    int n = sz(d);
    nxt.assign(n, vi(n, -1));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) if (d[i][j] < INFL) nxt[i][j] = j;
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            if (d[i][k] < INFL)
                for (int j = 0; j < n; j++)
                    if (d[k][j] < INFL && d[i][k] + d[k][j] < d[i][j]) {
                        d[i][j] = max(-INFL, d[i][k] + d[k][j]);
                        nxt[i][j] = nxt[i][k];
                    }
    for (int k = 0; k < n; k++)
        if (d[k][k] < 0)
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    if (d[i][k] < INFL && d[k][j] < INFL) d[i][j] = -INFL;
}

vi fpath(const vector<vi> &nxt, int i, int j) {
    vi p = {i};
    while (i != j) p.pb(i = nxt[i][j]);
    return p;
}

template <int N>
vector<bitset<N>> closure(vector<bitset<N>> r) {  // r[i][i] for reflexive
    int n = sz(r);
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            if (r[i][k]) r[i] |= r[k];
    return r;
}

vector<vl> minimax(vector<vl> d) {
    int n = sz(d);
    for (int k = 0; k < n; k++)
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                d[i][j] = min(d[i][j], max(d[i][k], d[k][j]));
    return d;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_floyd() {
    const ll X = INFL;
    vector<vl> d = {{0, 3, X, 7}, {8, 0, 2, X}, {5, X, 0, 1}, {2, X, X, 0}};
    vector<vi> nxt;
    floyd(d, nxt);
    vi p = fpath(nxt, 1, 3);  // 1 -> 2 -> 3
    // d[3][1]: 3 -> 0 -> 1 = 2 + 3; the path 1 2 3 has 3 nodes, p[1] = 2
    cout << d[1][3] << ' ' << d[3][1] << ' '  // 3 5 32
         << sz(p) << p[1] << '\n';
    vector<vl> w = {{0, 5, 9}, {5, 0, 2}, {9, 2, 0}};
    cout << minimax(w)[0][2] << '\n';  // 5
}
