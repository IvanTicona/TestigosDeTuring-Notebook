/**
 * Usage: vi mt = general_matching(n, E);  undirected, any graph
 *        mt[v] = partner of v or -1; matching size = #(mt[v] >= 0) / 2
 * Complexity: O(V^3) (Edmonds' blossom, BFS per free vertex); n <= ~500
 * Source: own (Edmonds blossom, e-maxx formulation); missing in all three
 *         references
 * Note: max independent set / min vertex cover have no König formula off
 *       bipartite graphs. Min edge cover = n - max matching (no isolated
 *       vertices). Perfect matching exists iff size == n / 2 (Tutte).
 *       Weighted general matching: not here (very long).
 * Status: ok
 */
vi general_matching(int n, const vector<pii> &E) {
    vector<vi> g(n);
    for (auto [u, v] : E) if (u != v) g[u].pb(v), g[v].pb(u);
    vi mt(n, -1), p(n), base(n);
    vector<char> used(n), blos(n);
    auto lca = [&](int a, int b) {
        vector<char> seen(n, 0);
        while (true) {  // climb from a through matched pairs
            a = base[a], seen[a] = 1;
            if (mt[a] < 0) break;
            a = p[mt[a]];
        }
        while (true) {
            b = base[b];
            if (seen[b]) return b;
            b = p[mt[b]];
        }
    };
    auto mark = [&](int v, int b, int child) {
        while (base[v] != b) {
            blos[base[v]] = blos[base[mt[v]]] = 1;
            p[v] = child, child = mt[v], v = p[mt[v]];
        }
    };
    auto find_path = [&](int root) {
        fill(all(used), 0), fill(all(p), -1);
        iota(all(base), 0);
        queue<int> q;
        used[root] = 1, q.push(root);
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int to : g[v]) {
                if (base[v] == base[to] || mt[v] == to) continue;
                if (to == root || (mt[to] >= 0 && p[mt[to]] >= 0)) {  // blossom
                    int cb = lca(v, to);
                    fill(all(blos), 0);
                    mark(v, cb, to), mark(to, cb, v);
                    for (int i = 0; i < n; i++)
                        if (blos[base[i]]) {
                            base[i] = cb;
                            if (!used[i]) used[i] = 1, q.push(i);
                        }
                } else if (p[to] < 0) {
                    p[to] = v;
                    if (mt[to] < 0) return to;  // augmenting path found
                    used[mt[to]] = 1, q.push(mt[to]);
                }
            }
        }
        return -1;
    };
    for (int v = 0; v < n; v++)
        if (mt[v] < 0)
            for (int u = find_path(v); u >= 0;) {  // flip the path
                int pv = p[u], nx = mt[pv];
                mt[u] = pv, mt[pv] = u, u = nx;
            }
    return mt;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_general_matching() {
    // odd cycle 0-1-2-3-4 plus vertex 5 hanging from 2: perfect matching
    vector<pii> E = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 0}, {2, 5}};
    vi mt = general_matching(6, E);
    int k = 0;
    for (int v = 0; v < 6; v++) k += mt[v] >= 0;
    cout << k / 2 << ' ' << mt[5] << '\n';  // 3 2
}
