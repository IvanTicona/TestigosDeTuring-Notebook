/**
 * Usage: E = {u, v, w} directed edges, nodes 0..n-1
 *        bellman_ford(n, E, s) -> dist: INFL unreachable, -INFL if a
 *                                 negative cycle can make it arbitrarily low
 *        neg_cycle(n, E)       -> vertices of SOME negative cycle (in order,
 *                                 anywhere in the graph), {} if none
 * Complexity: O(n m), stops early once nothing relaxes
 * Source: UMSS BellmanFord + UMSS BellmanFord Find Negative Cycle + UPB
 *         BellmanFord_SPFA (SPFA loops forever on a negative cycle: the
 *         counter only marks, never stops relaxing)
 * Note: needs |w| * n < INFL (distances are clamped at -INFL).
 *       Difference constraints x_v - x_u <= w: edge u -> v with weight w,
 *       add a source with 0-edges to all; infeasible iff negative cycle;
 *       x = dist. Longest path: negate.
 * Status: untested
 */
vl bellman_ford(int n, const vector<array<ll, 3>> &E, int s) {
    vl d(n, INFL);
    d[s] = 0;
    for (int r = 0; r < n; r++) {
        bool ch = false;  // clamp: a negative cycle keeps lowering forever
        for (auto [u, v, w] : E)
            if (d[u] < INFL && d[u] + w < d[v])
                d[v] = max(-INFL, d[u] + w), ch = 1;
        if (!ch) return d;  // converged: no negative cycle reachable
    }
    for (int r = 0; r < n; r++) {  // spread -INFL through everything reachable
        bool ch = false;
        for (auto [u, v, w] : E)
            if (d[u] < INFL && d[v] > -INFL &&
                (d[u] == -INFL || d[u] + w < d[v]))
                d[v] = -INFL, ch = 1;
        if (!ch) break;
    }
    return d;
}

vi neg_cycle(int n, const vector<array<ll, 3>> &E) {
    vl d(n, 0);  // virtual source to every node
    vi par(n, -1);
    int x = -1;
    for (int r = 0; r < n; r++) {
        x = -1;
        for (auto [u, v, w] : E)
            if (d[u] + w < d[v])
                d[v] = max(-INFL, d[u] + w), par[v] = u, x = v;
        if (x < 0) return {};
    }
    for (int i = 0; i < n; i++) x = par[x];  // surely inside the cycle now
    vi cyc;
    for (int v = x;; v = par[v]) {
        cyc.pb(v);
        if (v == x && sz(cyc) > 1) break;
    }
    cyc.pop_back();
    reverse(all(cyc));
    return cyc;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_bellman_ford() {
    vector<array<ll, 3>> E = {{0, 1, 4}, {0, 2, 5}, {1, 2, -3}, {2, 3, 2},
                              {3, 4, -1}, {4, 3, -1}};  // 3 <-> 4 negative
    vl d = bellman_ford(6, E, 0);
    bool minus = d[4] == -INFL, none = d[5] == INFL;
    cout << d[2] << ' ' << minus << ' ' << none << '\n';  // 1 1 1
    vi c = neg_cycle(6, E);
    cout << sz(c) << ' ' << min(c[0], c[1]) << max(c[0], c[1]) << '\n';  // 2 34
}
