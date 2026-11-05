/**
 * Usage: HLD h(g, root);  then put a segment tree / BIT over positions:
 *        value of vertex v lives at h.pos[v]
 *        h.path(u, v, f)        calls f(l, r) for O(log n) disjoint ranges
 *                               [l, r] covering the u - v path
 *        h.path(u, v, f, true)  same but EDGES (edge value stored at the
 *                               deeper endpoint): the lca is excluded
 *        h.subtree(v) -> {l, r};  h.lca(u, v)
 * Complexity: O(n) build; a path = O(log n) ranges -> O(log^2 n) with a
 *             segment tree; iterative build (no recursion)
 * Source: UMSS Heavy-Light (recursive, globals) + UPB
 *         HeavyLightDecomposition + UPB isc ds_heavy_light_decomposition +
 *         UTP heavy light
 * Note: ranges come in no particular order: for NON-commutative ops,
 *       split the walk into the u side and the v side. Subtree ranges and
 *       path ranges use the same positions (subtree + path in one tree).
 *       Forests work; path / lca need u and v in the same tree.
 * Status: untested
 */
struct HLD {
    vi par, dep, heavy, head, pos, sub;
    HLD(const vector<vi> &g, int root = 0) {  // forest: root first, then
        int n = sz(g);                         // the other trees by index
        par.assign(n, -1), dep.assign(n, 0), heavy.assign(n, -1);
        head.assign(n, 0), pos.assign(n, 0), sub.assign(n, 1);
        vi ord, roots;
        vector<char> seen(n, 0);
        for (int k = -1; k < n; k++) {
            int r = k < 0 ? root : k;
            if (r >= n || seen[r]) continue;
            seen[r] = 1, roots.pb(r), ord.pb(r);
            for (int i = sz(ord) - 1; i < sz(ord); i++)
                for (int v : g[ord[i]])
                    if (!seen[v])
                        seen[v] = 1, par[v] = ord[i],
                        dep[v] = dep[ord[i]] + 1, ord.pb(v);
        }
        for (int i = n - 1; i >= 0; i--)
            if (par[ord[i]] >= 0) sub[par[ord[i]]] += sub[ord[i]];
        for (int u : ord) {
            int best = 0;
            for (int v : g[u])
                if (v != par[u] && sub[v] > best) best = sub[v], heavy[u] = v;
        }
        int cur = 0;
        for (int r : roots) {
            vi st = {r};
            head[r] = r;
            while (!st.empty()) {  // preorder, heavy child right after parent
                int u = st.back();
                st.pop_back();
                pos[u] = cur++;
                for (int v : g[u])
                    if (v != par[u] && v != heavy[u]) head[v] = v, st.pb(v);
                if (heavy[u] >= 0) head[heavy[u]] = head[u], st.pb(heavy[u]);
            }
        }
    }
    template <class F>
    void path(int u, int v, F f, bool edges = false) const {
        for (; head[u] != head[v]; u = par[head[u]]) {
            if (dep[head[u]] < dep[head[v]]) swap(u, v);
            f(pos[head[u]], pos[u]);
        }
        if (dep[u] > dep[v]) swap(u, v);
        if (pos[u] + edges <= pos[v]) f(pos[u] + edges, pos[v]);
    }
    pii subtree(int v) const { return {pos[v], pos[v] + sub[v] - 1}; }
    int lca(int u, int v) const {
        for (; head[u] != head[v]; u = par[head[u]])
            if (dep[head[u]] < dep[head[v]]) swap(u, v);
        return dep[u] < dep[v] ? u : v;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_hld() {
    vector<vi> g = {{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    HLD h(g, 0);
    vl a(5);  // value of vertex v = v + 1, stored at pos[v]
    for (int v = 0; v < 5; v++) a[h.pos[v]] = v + 1;
    ll s = 0;
    h.path(3, 2, [&](int l, int r) { for (int i = l; i <= r; i++) s += a[i]; });
    auto [l, r] = h.subtree(1);
    // path 3 - 1 - 0 - 2 = 4 + 2 + 1 + 3
    cout << s << ' ' << r - l + 1 << ' ' << h.lca(3, 4) << '\n';  // 10 3 1
}
