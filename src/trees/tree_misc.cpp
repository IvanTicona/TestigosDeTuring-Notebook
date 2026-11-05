/**
 * Usage: g[u] = {(v, w)} weighted tree, n >= 1 (w = 1 for plain trees)
 *        diameter(g) -> {length, path of vertices}
 *        centers(g)  -> 1 or 2 vertices minimizing the max distance (by
 *                       #edges; uses w = 1 semantics)
 *        TreeIso ti; ti.label(adj, root) -> canonical id of the rooted
 *          tree (equal ids <=> isomorphic, same TreeIso object);
 *          ti.iso(a, b) -> unrooted isomorphism through the centers
 *        prufer(adj) / from_prufer(code) -> edges; labeled trees on n
 *          vertices: n^(n-2) (each code once)
 * Complexity: diameter / centers O(n); label O(n log n); prufer O(n log n)
 * Source: own (diameter by double search; centers, AHU isomorphism and
 *         Prufer are missing in all three references)
 * Note: diameter: farthest from any vertex, then farthest from that one
 *       (needs w >= 0). Center = middle of a diameter. Rerooting gives
 *       all eccentricities (dp/rerooting.cpp).
 * Status: untested
 */
pair<vl, vi> far_from(const vector<vector<pll>> &g, int s) {  // dist, par
    vl d(sz(g), -1);
    vi par(sz(g), -1), st = {s};
    d[s] = 0;
    while (!st.empty()) {
        int u = st.back();
        st.pop_back();
        for (auto [v, w] : g[u])
            if (d[v] < 0) d[v] = d[u] + w, par[v] = u, st.pb(v);
    }
    return {d, par};
}

pair<ll, vi> diameter(const vector<vector<pll>> &g) {
    auto d0 = far_from(g, 0).F;
    int a = max_element(all(d0)) - d0.begin();
    auto [d, par] = far_from(g, a);
    int b = max_element(all(d)) - d.begin();
    vi path;
    for (int v = b; v != -1; v = par[v]) path.pb(v);
    return {d[b], path};
}

vi centers(const vector<vector<pll>> &g) {
    vector<vector<pll>> h(sz(g));  // unit weights
    for (int u = 0; u < sz(g); u++) for (auto [v, w] : g[u]) h[u].pb({v, 1});
    vi p = diameter(h).S;
    int k = sz(p);
    if (k % 2) return {p[k / 2]};
    return {p[k / 2 - 1], p[k / 2]};
}

struct TreeIso {
    map<vi, int> ids;  // sorted children labels -> label
    int label(const vector<vi> &adj, int root) {
        int n = sz(adj);
        vi ord = {root}, par(n, -1), lab(n);
        for (int i = 0; i < sz(ord); i++)
            for (int v : adj[ord[i]])
                if (v != par[ord[i]]) par[v] = ord[i], ord.pb(v);
        for (int i = n - 1; i >= 0; i--) {
            int u = ord[i];
            vi ch;
            for (int v : adj[u]) if (v != par[u]) ch.pb(lab[v]);
            sort(all(ch));
            auto it = ids.find(ch);  // new shape: next id (args first)
            if (it == ids.end()) it = ids.emplace(ch, sz(ids)).first;
            lab[u] = it->S;
        }
        return lab[root];
    }
    bool iso(const vector<vi> &a, const vector<vi> &b) {
        if (sz(a) != sz(b)) return false;
        auto wrap = [](const vector<vi> &x) {
            vector<vector<pll>> h(sz(x));
            for (int u = 0; u < sz(x); u++) for (int v : x[u]) h[u].pb({v, 1});
            return h;
        };
        int la = label(a, centers(wrap(a))[0]);
        for (int c : centers(wrap(b))) if (label(b, c) == la) return true;
        return false;
    }
};

vi prufer(const vector<vi> &adj) {  // n >= 2, labels 0..n-1
    int n = sz(adj);
    vi deg(n);
    set<int> leaves;
    for (int v = 0; v < n; v++)
        if ((deg[v] = sz(adj[v])) == 1) leaves.insert(v);
    vector<char> gone(n, 0);
    vi code;
    for (int k = 0; k < n - 2; k++) {
        int l = *leaves.begin();
        leaves.erase(leaves.begin()), gone[l] = 1;
        for (int v : adj[l])
            if (!gone[v]) {
                code.pb(v);
                if (--deg[v] == 1) leaves.insert(v);
            }
    }
    return code;
}

vector<pii> from_prufer(const vi &code) {
    int n = sz(code) + 2;
    vi deg(n, 1);
    for (int x : code) deg[x]++;
    set<int> leaves;
    for (int v = 0; v < n; v++) if (deg[v] == 1) leaves.insert(v);
    vector<pii> es;
    for (int x : code) {
        int l = *leaves.begin();
        leaves.erase(leaves.begin());
        es.pb({l, x});
        if (--deg[x] == 1) leaves.insert(x);
    }
    es.pb({*leaves.begin(), *leaves.rbegin()});
    return es;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_tree_misc() {
    vector<vector<pll>> g(5);
    auto add = [&](int u, int v, ll w) { g[u].pb({v, w}), g[v].pb({u, w}); };
    add(0, 1, 2), add(1, 2, 3), add(1, 3, 4), add(3, 4, 1);  // 2-1-3-4: 8
    auto [len, path] = diameter(g);
    // unweighted diameter 0-1-3-4 has 4 vertices -> 2 centers (1 and 3)
    cout << len << ' ' << sz(path) << ' ' << sz(centers(g)) << '\n';  // 8 4 2
    vector<vi> a = {{1}, {0, 2}, {1}}, b = {{2}, {2}, {0, 1}};  // 2 paths
    TreeIso ti;
    vi code = prufer({{3}, {3}, {3}, {0, 1, 2}});  // star centered at 3
    int m = sz(from_prufer({3, 3}));  // the star again: 3 edges
    cout << ti.iso(a, b) << ' ' << code[0] << code[1] << ' '  // 1 33 3
         << m << '\n';
}
