/**
 * Usage: auto [vs, es] = euler(n, E, directed, src = -1);
 *        E[i] = {u, v}; vs = vertices of the trail (m + 1 of them), es =
 *        edge ids in order; both empty if no Eulerian trail exists (or none
 *        starting at src, when src >= 0). Circuit iff vs.front() == vs.back()
 * Complexity: O(n + m), iterative Hierholzer
 * Source: UPB EulerianPath (undirected only; started at vertex 0 when no
 *         odd vertex existed, even if 0 was isolated) + UTP eulerian path;
 *         directed version, src and edge order: own
 * Note: undirected: 0 or 2 odd-degree vertices; directed: out - in = 1 at
 *       the start, -1 at the end, 0 elsewhere; plus all edges connected
 *       (checked through the length). De Bruijn sequence: Euler circuit on
 *       the (k-1)-mers.
 * Status: untested
 */
pair<vi, vi> euler(int n, const vector<pii> &E, bool directed, int src = -1) {
    int m = sz(E);
    vector<vector<pii>> g(n);
    vi bal(n, 0);  // directed: out - in; undirected: degree
    for (int i = 0; i < m; i++) {
        auto [u, v] = E[i];
        g[u].pb({v, i}), bal[u]++;
        if (directed) bal[v]--;
        else g[v].pb({u, i}), bal[v]++;
    }
    int start = -1, odd = 0;
    for (int v = 0; v < n; v++) {
        bool special = directed ? bal[v] != 0 : bal[v] % 2;
        odd += special;
        if (directed && bal[v] == 1) start = v;
        if (!directed && special && start < 0) start = v;
    }
    if (odd > 2 || (directed && odd && start < 0)) return {};
    if (start < 0)  // circuit: any vertex with an edge (or src)
        for (int v = 0; v < n; v++) if (!g[v].empty()) { start = v; break; }
    if (m == 0) return {{src >= 0 ? src : 0}, {}};
    if (src >= 0) {
        if (odd && src != start) {
            if (directed || bal[src] % 2 == 0) return {};
        } else if (!odd && g[src].empty()) return {};
        start = src;
    }
    vi it(n, 0), vs, es;
    vector<char> used(m, 0);
    vector<pii> st = {{start, -1}};
    while (!st.empty()) {
        auto [u, e] = st.back();
        while (it[u] < sz(g[u]) && used[g[u][it[u]].S]) it[u]++;
        if (it[u] == sz(g[u])) {
            vs.pb(u), st.pop_back();
            if (e >= 0) es.pb(e);
        } else {
            auto [v, id] = g[u][it[u]++];
            used[id] = 1, st.pb({v, id});
        }
    }
    if (sz(es) != m) return {};
    reverse(all(vs)), reverse(all(es));
    return {vs, es};
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_euler() {
    vector<pii> E = {{0, 1}, {1, 2}, {2, 0}, {2, 3}};  // triangle + tail
    auto [vs, es] = euler(5, E, false);  // vertex 4 isolated: fine
    cout << sz(vs) << ' ' << vs.front() << vs.back() << '\n';  // 5 23
    auto [dv, de] = euler(3, {{0, 1}, {1, 2}, {2, 0}}, true, 1);
    cout << dv[0] << dv[1] << dv[2] << dv[3] << ' ' << de[0] << '\n';  // 1201 1
}
