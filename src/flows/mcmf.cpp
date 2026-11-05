/**
 * Usage: MCMF m(n); int id = m.add_edge(u, v, cap, cost);
 *        auto [flow, cost] = m.solve(s, t, maxf = INFL);  (min cost among
 *        max flows, or among flows of value maxf); m.flow(id)
 * Complexity: O(F (E log V)) with F = #augmentations (<= flow value);
 *             first potentials by Bellman-Ford O(V E)
 * Source: UPB MinCostMaxFlowPotentials (verified) + UPB MinCostMaxFlow +
 *         UPB isc graph_mincost_maxflow + UMSS Min Cost Max Flow (CheapDinitz:
 *         did not compile, int costs)
 * Note: negative costs are fine if there is no negative cycle. Max cost:
 *       negate. Min cost for EVERY flow value k: record cost after each
 *       augmentation (cost per unit is non-decreasing). Assignment with
 *       n <= ~500: hungarian.cpp is faster.
 * Status: untested
 */
struct MCMF {
    struct E { int to; ll cap, cost; };
    vector<E> e;
    vl cap0;
    vector<vi> g;
    MCMF(int n) : g(n) {}
    int add_edge(int u, int v, ll cap, ll cost) {
        g[u].pb(sz(e)), e.pb({v, cap, cost}), cap0.pb(cap);
        g[v].pb(sz(e)), e.pb({u, 0, -cost}), cap0.pb(0);
        return sz(e) - 2;
    }
    ll flow(int id) const { return cap0[id] - e[id].cap; }
    pll solve(int s, int t, ll maxf = INFL) {
        int n = sz(g);
        vl pot(n, INFL), d(n);
        vi pe(n);
        pot[s] = 0;
        for (int r = 0; r < n; r++)  // Bellman-Ford: potentials for < 0 costs
            for (int i = 0; i < sz(e); i++)
                if (int u = e[i ^ 1].to; e[i].cap > 0 && pot[u] < INFL)
                    pot[e[i].to] = min(pot[e[i].to], pot[u] + e[i].cost);
        for (ll &x : pot) if (x == INFL) x = 0;
        ll flow = 0, cost = 0;
        while (flow < maxf) {
            d.assign(n, INFL);
            priority_queue<pll, vector<pll>, greater<pll>> pq;
            d[s] = 0, pq.push({0, s});
            while (!pq.empty()) {
                auto [du, u] = pq.top();
                pq.pop();
                if (du > d[u]) continue;
                for (int id : g[u]) {
                    auto [v, cp, c] = e[id];
                    ll nd = du + c + pot[u] - pot[v];
                    if (cp > 0 && nd < d[v])
                        d[v] = nd, pe[v] = id, pq.push({nd, v});
                }
            }
            if (d[t] == INFL) break;
            for (int v = 0; v < n; v++) if (d[v] < INFL) pot[v] += d[v];
            ll f = maxf - flow;
            for (int v = t; v != s; v = e[pe[v] ^ 1].to)
                f = min(f, e[pe[v]].cap);
            for (int v = t; v != s; v = e[pe[v] ^ 1].to) {
                int id = pe[v];
                e[id].cap -= f, e[id ^ 1].cap += f, cost += f * e[id].cost;
            }
            flow += f;
        }
        return {flow, cost};
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_mcmf() {
    MCMF m(4);
    m.add_edge(0, 1, 2, 1), m.add_edge(0, 2, 1, 2);
    int a = m.add_edge(1, 3, 1, 3);
    m.add_edge(1, 2, 1, 1), m.add_edge(2, 3, 2, 1);
    auto [f, c] = m.solve(0, 3);  // paths 0-1-2-3 (3), 0-2-3 (3), 0-1-3 (4)
    cout << f << ' ' << c << ' ' << m.flow(a) << '\n';  // 3 10 1
}
