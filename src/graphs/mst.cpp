/**
 * Usage: E[i] = {w, u, v} (undirected)
 *        auto [cost, ids] = kruskal(n, E)  -> min spanning FOREST cost and
 *          chosen edge ids (n - #components of them); max: negate weights
 *        prim_dense(w) -> MST cost of a complete graph given by a matrix
 *          (INFL = no edge; -1 if disconnected), O(n^2): e.g. points
 * Complexity: kruskal O(m log m); prim_dense O(n^2)
 * Source: UMSS Kruskal (always max: reverse() left in) + UPB Kruskal +
 *         UPB MST_Kruskal_Prim (critical edges wrong) + UPB Prim + old
 *         minimax_mst_path; dense Prim: own
 * Note: MST minimizes the max edge on every path (minimax paths); second
 *       best MST: try each non-tree edge + max edge on its tree path (LCA).
 *       Edge e is in SOME MST iff it is not the unique max on a cycle:
 *       process equal weights as a group. Manhattan MST: 4 sweeps, O(n log).
 * Depends: src/ds/dsu.cpp
 * Status: untested
 */
pair<ll, vi> kruskal(int n, const vector<array<ll, 3>> &E) {
    vi ord(sz(E));
    iota(all(ord), 0);
    sort(all(ord), [&](int a, int b) { return E[a][0] < E[b][0]; });
    DSU d(n);
    ll cost = 0;
    vi ids;
    for (int i : ord)
        if (d.unite(E[i][1], E[i][2])) cost += E[i][0], ids.pb(i);
    return {cost, ids};
}

ll prim_dense(const vector<vl> &w) {
    int n = sz(w);
    vl best(n, INFL);
    vector<char> in(n, 0);
    best[0] = 0;
    ll cost = 0;
    for (int it = 0; it < n; it++) {
        int u = -1;
        for (int v = 0; v < n; v++)
            if (!in[v] && (u < 0 || best[v] < best[u])) u = v;
        if (best[u] >= INFL) return -1;
        in[u] = 1, cost += best[u];
        for (int v = 0; v < n; v++) if (!in[v]) best[v] = min(best[v], w[u][v]);
    }
    return cost;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_mst() {
    vector<array<ll, 3>> E = {{4, 0, 1}, {1, 1, 2}, {3, 0, 2}, {2, 2, 3},
                              {5, 1, 3}};
    auto [c, ids] = kruskal(4, E);
    cout << c << ' ' << sz(ids) << ' ' << ids[0] << '\n';  // 6 3 1
    const ll X = INFL;
    vector<vl> w = {{0, 4, 3, X}, {4, 0, 1, 5}, {3, 1, 0, 2}, {X, 5, 2, 0}};
    cout << prim_dense(w) << '\n';  // 6
}
