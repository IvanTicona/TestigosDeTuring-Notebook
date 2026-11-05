/**
 * Usage: g[u] = {(v, w)}, w >= 0;  dist = INFL if unreachable
 *        auto [d, par] = dijkstra(g, {s1, s2, ...})  (multi-source)
 *        path: walk par[] back from t (par = -1 at sources)
 *        k_shortest(g, s, t, k) -> lengths of the k shortest WALKS s -> t
 *                                  (repeated vertices allowed), sorted
 * Complexity: O((n + m) log m); k_shortest O(k m log(k m))
 * Source: UPB FastDijkstra + UPB MultiSourceDijkstra + UMSS Dijkstra
 *         (std::distance name clash) + UMSS Dijkstra K-Shortest Path; UPB
 *         KShortestPaths_Yen (simple paths): not included
 * Note: lazy deletion (skip stale queue entries). Count shortest paths /
 *       min edges among shortest: relax with == too. Dense graph (m ~ n^2):
 *       O(n^2) array version is faster. Potentials (Johnson) for negative
 *       edges without negative cycles: w'(u,v) = w + h(u) - h(v).
 * Status: untested
 */
pair<vl, vi> dijkstra(const vector<vector<pll>> &g, const vi &src) {
    vl d(sz(g), INFL);
    vi par(sz(g), -1);
    priority_queue<pll, vector<pll>, greater<pll>> pq;
    for (int s : src) d[s] = 0, pq.push({0, s});
    while (!pq.empty()) {
        auto [du, u] = pq.top();
        pq.pop();
        if (du > d[u]) continue;
        for (auto [v, w] : g[u])
            if (du + w < d[v]) d[v] = du + w, par[v] = u, pq.push({d[v], v});
    }
    return {d, par};
}

vl k_shortest(const vector<vector<pll>> &g, int s, int t, int k) {
    vi cnt(sz(g), 0);
    vl res;
    priority_queue<pll, vector<pll>, greater<pll>> pq;
    pq.push({0, s});
    while (!pq.empty() && sz(res) < k) {
        auto [du, u] = pq.top();
        pq.pop();
        if (cnt[u] >= k) continue;  // each node is popped at most k times
        cnt[u]++;
        if (u == t) res.pb(du);
        for (auto [v, w] : g[u]) pq.push({du + w, v});
    }
    return res;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_dijkstra() {
    vector<vector<pll>> g(4);
    auto add = [&](int u, int v, ll w) { g[u].pb({v, w}), g[v].pb({u, w}); };
    add(0, 1, 4), add(0, 2, 1), add(2, 1, 2), add(1, 3, 5);
    auto [d, par] = dijkstra(g, {0});
    cout << d[1] << ' ' << d[3] << ' ' << par[1] << '\n';  // 3 8 2
    vl k = k_shortest(g, 0, 3, 3);  // 0-2-1-3, 0-1-3, 0-2-0-2-1-3 ...
    cout << k[0] << ' ' << k[1] << ' ' << k[2] << '\n';    // 8 9 10
}
