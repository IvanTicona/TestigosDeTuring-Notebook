/**
 * Usage: E[i] = {u, v, lo, hi}: flow on edge i must be in [lo, hi]
 *        vl f; ll F = flow_lower(n, E, s, t, f) -> max s-t flow value, -1
 *          if no feasible flow; f[i] = flow on edge i
 *        flow_lower(n, E, -1, -1, f) -> feasible circulation? (0 / -1)
 * Complexity: two max flows (Dinic)
 * Source: own (classic reduction: demands + super source / sink); missing
 *         in all three references
 * Note: min flow instead of max: after the feasibility step, push back
 *       from t to s (maxflow(t, s)) and subtract. Each lo is forced:
 *       excess lo at v, deficit lo at u; t -> s edge of capacity INF turns
 *       the s-t flow into a circulation.
 * Depends: src/flows/dinic.cpp
 * Status: ok
 */
ll flow_lower(int n, const vector<array<ll, 4>> &E, int s, int t, vl &f) {
    Dinic d(n + 2);
    int S = n, T = n + 1;
    vl dem(n, 0);
    vi ids;
    for (auto [u, v, lo, hi] : E) {
        ids.pb(d.add_edge(u, v, hi - lo));
        dem[v] += lo, dem[u] -= lo;
    }
    int ts = s >= 0 ? d.add_edge(t, s, INFL) : -1;
    ll need = 0;
    for (int v = 0; v < n; v++)
        if (dem[v] > 0) d.add_edge(S, v, dem[v]), need += dem[v];
        else if (dem[v] < 0) d.add_edge(v, T, -dem[v]);
    if (d.maxflow(S, T) != need) return -1;
    ll F = 0;
    if (s >= 0) {
        F = d.flow(ts);
        d.e[ts].cap = d.e[ts ^ 1].cap = 0;  // remove t -> s, keep its flow
        F += d.maxflow(s, t);
    }
    f.assign(sz(E), 0);
    for (int i = 0; i < sz(E); i++) f[i] = E[i][2] + d.flow(ids[i]);
    return F;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_lower_bounds() {
    // 0 -> 1 -> 2 must carry at least 2 on 1 -> 2, the bypass 0 -> 2 <= 1
    vector<array<ll, 4>> E = {{0, 1, 0, 5}, {1, 2, 2, 3}, {0, 2, 0, 1}};
    vl f;
    ll F = flow_lower(3, E, 0, 2, f);
    cout << F << ' ' << f[0] << f[1] << f[2] << '\n';  // 4 331
    vector<array<ll, 4>> C = {{0, 1, 2, 2}, {1, 0, 0, 1}};  // impossible
    cout << flow_lower(2, C, -1, -1, f) << '\n';         // -1
}
