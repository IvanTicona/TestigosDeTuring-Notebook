/**
 * Usage: g = adjacency list (vector<vi>), dist = -1 if unreachable
 *        auto [d, par] = bfs(g, {s1, s2, ...})  multi-source BFS
 *        get_path(par, t, d)       -> nodes from a source to t ({} if none)
 *        bfs01(g01, s)             -> distances with weights 0 / 1
 *                                     (g01[u] = {(v, w)}, deque)
 *        bipartite(g)              -> color 0/1 per node, empty if odd cycle
 * Complexity: O(n + m)
 * Source: UMSS BFS + UPB BFS01 + UMSS DFS (never left the start node);
 *         multi-source, paths, bipartite: own
 * Note: grids: node = r * C + c, neighbors with dx / dy from the template.
 *       0-1 BFS also covers "cost 1 to change direction / break a wall".
 *       Shortest cycle through BFS from every node: O(n m).
 * Status: ok
 */
pair<vi, vi> bfs(const vector<vi> &g, const vi &src) {
    vi d(sz(g), -1), par(sz(g), -1);
    queue<int> q;
    for (int s : src) if (d[s] < 0) d[s] = 0, q.push(s);
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : g[u])
            if (d[v] < 0) d[v] = d[u] + 1, par[v] = u, q.push(v);
    }
    return {d, par};
}

vi get_path(const vi &par, int t, const vi &d) {
    if (d[t] < 0) return {};
    vi p;
    for (int v = t; v != -1; v = par[v]) p.pb(v);
    reverse(all(p));
    return p;
}

vl bfs01(const vector<vector<pii>> &g, int s) {
    vl d(sz(g), INFL);
    deque<int> dq;
    d[s] = 0, dq.pb(s);
    while (!dq.empty()) {
        int u = dq.front();
        dq.pop_front();
        for (auto [v, w] : g[u])
            if (d[u] + w < d[v]) {
                d[v] = d[u] + w;
                w ? dq.pb(v) : dq.push_front(v);
            }
    }
    return d;  // INFL = unreachable
}

vi bipartite(const vector<vi> &g) {
    vi c(sz(g), -1);
    for (int s = 0; s < sz(g); s++) {
        if (c[s] >= 0) continue;
        queue<int> q;
        c[s] = 0, q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int v : g[u]) {
                if (c[v] < 0) c[v] = c[u] ^ 1, q.push(v);
                else if (c[v] == c[u]) return {};
            }
        }
    }
    return c;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_bfs() {
    vector<vi> g = {{1, 2}, {0, 3}, {0, 3}, {1, 2, 4}, {3}, {}};  // 5 isolated
    auto [d, par] = bfs(g, {0});
    vi p = get_path(par, 4, d);
    cout << d[4] << ' ' << d[5] << ' ' << sz(p) << ' '  // 3 -1 4 04
         << p[0] << p[3] << '\n';
    vector<vector<pii>> h = {{{1, 1}, {2, 0}}, {{3, 0}}, {{1, 0}}, {}};
    cout << bfs01(h, 0)[3] << ' ' << sz(bipartite(g)) << '\n';  // 0 6
}
