/**
 * Usage: auto t = gomory_hu(n, E);  undirected, E[i] = {u, v, cap}
 *        t[v] = {parent, w} for v >= 1 (tree on n nodes, root 0): the min
 *        u-v cut of the graph = min weight on the tree path u -> v
 *        gh_min_cut(t, u, v) -> that value (O(n) walk)
 * Complexity: n - 1 max flows (Dinic)
 * Source: UPB GomoryHuTree (Gusfield, rebuilt Dinic each time, extra swap
 *         step); own (plain Gusfield: flow tree with the same cut values)
 * Note: all-pairs min cut values in O(n) flows. The tree edges are cuts of
 *       the graph only up to equal values (Gusfield gives a flow-equivalent
 *       tree). Sum over pairs of min cut: process tree edges in decreasing
 *       weight with DSU (each edge is the min for |A| * |B| pairs).
 * Depends: src/flows/dinic.cpp
 * Status: untested
 */
vector<pair<int, ll>> gomory_hu(int n, const vector<array<ll, 3>> &E) {
    vector<pair<int, ll>> t(n, {0, 0});
    for (int i = 1; i < n; i++) {
        Dinic d(n);
        for (auto [u, v, c] : E) d.add_edge(u, v, c, c);
        int p = t[i].F;
        t[i].S = d.maxflow(i, p);
        for (int j = i + 1; j < n; j++)
            if (t[j].F == p && d.cut_side(j)) t[j].F = i;
    }
    return t;
}

ll gh_min_cut(const vector<pair<int, ll>> &t, int u, int v) {
    int n = sz(t);
    vi dep(n, -1);  // depth by walking parents (tree rooted at 0)
    auto depth = [&](auto &&self, int x) -> int {
        if (dep[x] >= 0) return dep[x];
        return dep[x] = x == 0 ? 0 : self(self, t[x].F) + 1;
    };
    ll r = INFL;
    while (u != v) {
        if (depth(depth, u) < depth(depth, v)) swap(u, v);
        r = min(r, t[u].S), u = t[u].F;
    }
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_gomory_hu() {
    vector<array<ll, 3>> E = {{0, 1, 3}, {1, 2, 1}, {2, 3, 4}, {0, 3, 2}};
    auto t = gomory_hu(4, E);  // cycle 0-1-2-3-0
    // a cut on a cycle takes one edge of each side: 3 + 1, 1 + 2, 4 + 1
    cout << gh_min_cut(t, 0, 1) << ' ' << gh_min_cut(t, 1, 2) << ' '  // 4 3 5
         << gh_min_cut(t, 2, 3) << '\n';
}
