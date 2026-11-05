/**
 * Usage: LCA t(g, root) with g[u] = {(v, w)} (w = edge weight, any);
 *        t.lca(u, v), t.dist(u, v) (sum of w), t.depth[v] (#edges),
 *        t.kth_anc(u, k) (-1 if too high), t.kth_on_path(u, v, k) (k-th
 *        vertex from u, k = 0 -> u), t.path_max(u, v) (max edge weight)
 * Complexity: build O(n log n), queries O(log n); iterative BFS (no
 *             recursion)
 * Source: UMSS LCA + UMSS K-th Parent + UPB LCA + UPB BinaryLiftingExtras
 *         (recursive dfs) + old lca_binary_lifting (quarantined) + old
 *         minimax_mst_path; path aggregate: own
 * Note: path min / sum / gcd: same table as mx with another operation.
 *       Minimax path between two nodes of a graph: MST + path_max.
 *       Forest: build from every root (or add a virtual root).
 * Status: untested
 */
struct LCA {
    int L = 1;
    vi depth;
    vl wdep;              // weighted depth
    vector<vi> up;        // up[k][v] = 2^k-th ancestor (root -> root)
    vector<vl> mx;        // max edge weight on that jump
    LCA(const vector<vector<pll>> &g, int root = 0) {
        int n = sz(g);
        while ((1 << L) < n) L++;
        depth.assign(n, 0), wdep.assign(n, 0);
        up.assign(L, vi(n, root)), mx.assign(L, vl(n, LLONG_MIN));
        vi q = {root};
        vector<char> seen(n, 0);
        seen[root] = 1;
        for (int i = 0; i < sz(q); i++) {
            int u = q[i];
            for (auto [v, w] : g[u])
                if (!seen[v]) {
                    seen[v] = 1, q.pb(v);
                    up[0][v] = u, mx[0][v] = w;
                    depth[v] = depth[u] + 1, wdep[v] = wdep[u] + w;
                }
        }
        for (int k = 1; k < L; k++)
            for (int v : q) {  // BFS order: ancestors are done first
                up[k][v] = up[k - 1][up[k - 1][v]];
                mx[k][v] = max(mx[k - 1][v], mx[k - 1][up[k - 1][v]]);
            }
    }
    int kth_anc(int u, int k) const {
        if (k > depth[u]) return -1;
        for (int i = 0; i < L; i++) if (k >> i & 1) u = up[i][u];
        return u;
    }
    int lca(int u, int v) const {
        if (depth[u] < depth[v]) swap(u, v);
        u = kth_anc(u, depth[u] - depth[v]);
        if (u == v) return u;
        for (int i = L - 1; i >= 0; i--)
            if (up[i][u] != up[i][v]) u = up[i][u], v = up[i][v];
        return up[0][u];
    }
    ll dist(int u, int v) const {
        return wdep[u] + wdep[v] - 2 * wdep[lca(u, v)];
    }
    int kth_on_path(int u, int v, int k) const {
        int a = lca(u, v), du = depth[u] - depth[a], dv = depth[v] - depth[a];
        if (k > du + dv) return -1;
        return k <= du ? kth_anc(u, k) : kth_anc(v, du + dv - k);
    }
    ll path_max(int u, int v) const {  // LLONG_MIN if u == v
        ll r = LLONG_MIN;
        if (depth[u] < depth[v]) swap(u, v);
        for (int i = L - 1; i >= 0; i--)
            if (depth[u] - (1 << i) >= depth[v])
                r = max(r, mx[i][u]), u = up[i][u];
        if (u == v) return r;
        for (int i = L - 1; i >= 0; i--)
            if (up[i][u] != up[i][v]) {
                r = max({r, mx[i][u], mx[i][v]});
                u = up[i][u], v = up[i][v];
            }
        return max({r, mx[0][u], mx[0][v]});
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_lca() {
    // 0 -(1)- 1 -(5)- 3, 0 -(2)- 2 -(7)- 4
    vector<vector<pll>> g(5);
    auto add = [&](int u, int v, ll w) { g[u].pb({v, w}), g[v].pb({u, w}); };
    add(0, 1, 1), add(1, 3, 5), add(0, 2, 2), add(2, 4, 7);
    LCA t(g, 0);
    cout << t.lca(3, 4) << ' ' << t.dist(3, 4) << ' '  // 0 15 7
         << t.path_max(3, 4) << '\n';
    cout << t.kth_on_path(3, 4, 3) << ' ' << t.kth_anc(4, 3) << '\n';  // 2 -1
}
