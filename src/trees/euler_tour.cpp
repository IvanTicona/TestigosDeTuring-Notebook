/**
 * Usage: EulerTour et(g, root);  g = tree adjacency (vector<vi>)
 *        et.tin[v], et.tout[v] -> subtree of v = preorder positions
 *          [tin, tout] (tout = tin + size - 1); et.ord[pos] = vertex
 *        et.is_anc(u, v) (u ancestor of v, u == v counts)
 *        et.lca(u, v) in O(1) (Euler sequence + sparse table on depths)
 * Complexity: O(n log n) build, O(1) queries; iterative DFS
 * Source: UMSS Euler Tour - Sum (CSES 1138; int Fenwick) + UPB EulerTour
 *         + UPB EulerTour_Queries + old lca_euler_segtree; O(1) LCA: own
 * Note: subtree update / query: segment tree or BIT on [tin, tout].
 *       Path root -> v sums with point updates: BIT range-add +x on
 *       [tin[v], tout[v]] when a[v] += x, then point query at tin[u].
 *       Path u - v = f(u) + f(v) - 2 f(lca) + a[lca].
 * Status: untested
 */
struct EulerTour {
    vi tin, tout, ord, dep, first, eul;
    vector<vi> sp;
    EulerTour(const vector<vi> &g, int root = 0) {
        int n = sz(g);
        tin = tout = dep = first = vi(n, 0);
        vi it(n, 0), par(n, -1), st = {root};
        tin[root] = 0, ord.pb(root), first[root] = 0, eul.pb(root);
        while (!st.empty()) {
            int u = st.back();
            if (it[u] < sz(g[u])) {
                int v = g[u][it[u]++];
                if (v == par[u]) continue;
                par[v] = u, dep[v] = dep[u] + 1, tin[v] = sz(ord), ord.pb(v);
                first[v] = sz(eul), eul.pb(v), st.pb(v);
            } else {
                tout[u] = sz(ord) - 1, st.pop_back();
                if (!st.empty()) eul.pb(st.back());
            }
        }
        int m = sz(eul);
        sp.assign(1, eul);
        for (int k = 1; (1 << k) <= m; k++) {
            sp.pb(vi(m - (1 << k) + 1));
            for (int i = 0; i < sz(sp[k]); i++) {
                int a = sp[k - 1][i], b = sp[k - 1][i + (1 << (k - 1))];
                sp[k][i] = dep[a] < dep[b] ? a : b;
            }
        }
    }
    bool is_anc(int u, int v) const {
        return tin[u] <= tin[v] && tout[v] <= tout[u];
    }
    int lca(int u, int v) const {
        int l = first[u], r = first[v];
        if (l > r) swap(l, r);
        int k = __lg(r - l + 1), a = sp[k][l], b = sp[k][r - (1 << k) + 1];
        return dep[a] < dep[b] ? a : b;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_euler_tour() {
    vector<vi> g = {{1, 2}, {0, 3, 4}, {0}, {1}, {1}};  // 0: {1, 2}, 1: {3, 4}
    EulerTour et(g, 0);
    cout << et.tin[1] << ' ' << et.tout[1] << ' '  // 1 3 10
         << et.lca(3, 4) << et.lca(4, 2) << '\n';
    cout << et.is_anc(1, 4) << et.is_anc(2, 4) << '\n';  // 10
}
