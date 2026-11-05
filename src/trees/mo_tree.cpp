/**
 * Usage: vl res = mo_tree(g, root, qs, toggle, get);
 *        qs[i] = {u, v}: path query; toggle(x) adds vertex x if absent,
 *        removes it otherwise; get() = answer for the current vertex set
 *        (exactly the vertices of the u - v path when get() is called)
 * Complexity: O((n + q) sqrt n) toggles
 * Source: UMSS Mo's in Trees + UPB MosOnTree_Hilbert ("Hilbert" was a
 *         Z-order key) + UTP Mo's on trees; generic version: own
 * Note: flattening with entry AND exit times (2n positions): a vertex is
 *       in the path iff it appears once in the range; the lca is added
 *       apart when it is not an endpoint. Subtree queries need no Mo: use
 *       the Euler tour ranges directly.
 * Depends: src/ds/mo.cpp
 * Depends: src/trees/euler_tour.cpp
 * Status: untested
 */
template <class T, class G>
vl mo_tree(const vector<vi> &g, int root, const vector<pii> &qs, T toggle,
           G get) {
    int n = sz(g), q = sz(qs);
    vi st(n), en(n), flat, it(n, 0), par(n, -1), stk = {root};
    st[root] = 0, flat.pb(root);
    while (!stk.empty()) {  // iterative DFS: entry and exit positions
        int u = stk.back();
        if (it[u] < sz(g[u])) {
            int v = g[u][it[u]++];
            if (v != par[u])
                par[v] = u, st[v] = sz(flat), flat.pb(v), stk.pb(v);
        } else en[u] = sz(flat), flat.pb(u), stk.pop_back();
    }
    EulerTour et(g, root);
    vector<array<int, 3>> rg(q);  // l, r, extra vertex (lca) or -1
    for (int i = 0; i < q; i++) {
        auto [u, v] = qs[i];
        if (st[u] > st[v]) swap(u, v);
        int w = et.lca(u, v);
        if (w == u) rg[i] = {st[u], st[v], -1};  // u is an ancestor of v
        else rg[i] = {en[u], st[v], w};
    }
    vi ord(q);
    iota(all(ord), 0);
    vl key(q);
    for (int i = 0; i < q; i++) key[i] = hilbert(rg[i][0], rg[i][1]);
    sort(all(ord), [&](int a, int b) { return key[a] < key[b]; });
    vl res(q);
    int L = 0, R = -1;
    for (int i : ord) {
        auto [l, r, w] = rg[i];
        while (R < r) toggle(flat[++R]);
        while (L > l) toggle(flat[--L]);
        while (R > r) toggle(flat[R--]);
        while (L < l) toggle(flat[L++]);
        if (w >= 0) toggle(w);
        res[i] = get();
        if (w >= 0) toggle(w);
    }
    return res;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_mo_tree() {
    vector<vi> g = {{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    vi col = {1, 2, 1, 2, 3}, cnt(4, 0);
    vector<char> in(5, 0);
    int distinct = 0;  // #distinct colors on the path
    auto toggle = [&](int x) {
        if (in[x] ^= 1) distinct += cnt[col[x]]++ == 0;
        else distinct -= --cnt[col[x]] == 0;
    };
    auto get = [&] { return distinct; };
    vl r = mo_tree(g, 0, {{3, 2}, {3, 4}, {2, 2}}, toggle, get);
    cout << r[0] << ' ' << r[1] << ' ' << r[2] << '\n';  // 2 2 1
}
