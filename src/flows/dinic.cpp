/**
 * Usage: Dinic d(n); int id = d.add_edge(u, v, cap, rcap = 0)  (rcap > 0:
 *        undirected / two-way); ll f = d.maxflow(s, t);
 *        d.flow(id) -> flow on that edge; d.cut_side(v) -> v on the
 *        source side of a min cut (after maxflow)
 * Complexity: O(V^2 E); unit capacities O(E sqrt V); fast in practice
 * Source: UPB Dinic (code/) + UMSS Max Flow + old dinic (KACTL, Benq
 *         macros: did not compile) + UPB isc graph_dinic (returned the
 *         wrong value); edge ids and cut side: own
 * Note: min cut edges = edges u -> v with cut_side(u) && !cut_side(v).
 *       Call maxflow again after adding edges: it continues from the
 *       current flow. Capacities up to ~1e18 (ll).
 * Status: untested
 */
struct Dinic {
    struct E { int to; ll cap; };
    vector<E> e;  // edge i and i ^ 1 are a pair
    vl cap0;      // original capacities
    vector<vi> g;
    vi lvl, it;
    Dinic(int n) : g(n) {}
    int add_edge(int u, int v, ll cap, ll rcap = 0) {
        g[u].pb(sz(e)), e.pb({v, cap}), cap0.pb(cap);
        g[v].pb(sz(e)), e.pb({u, rcap}), cap0.pb(rcap);
        return sz(e) - 2;
    }
    bool bfs(int s, int t) {
        lvl.assign(sz(g), -1);
        queue<int> q;
        lvl[s] = 0, q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int id : g[u])
                if (int v = e[id].to; e[id].cap > 0 && lvl[v] < 0)
                    lvl[v] = lvl[u] + 1, q.push(v);
        }
        return lvl[t] >= 0;
    }
    vi path;  // edge ids from s; iterative: no stack overflow on long paths
    ll augment(int s, int t) {
        path.clear();
        for (int u = s;;) {
            if (u == t) {
                ll d = LLONG_MAX;
                for (int id : path) d = min(d, e[id].cap);
                for (int id : path) e[id].cap -= d, e[id ^ 1].cap += d;
                return d;
            }
            int &i = it[u];
            while (i < sz(g[u]) &&
                   !(e[g[u][i]].cap > 0 && lvl[e[g[u][i]].to] == lvl[u] + 1))
                i++;
            if (i < sz(g[u])) path.pb(g[u][i]), u = e[g[u][i]].to;
            else if (path.empty()) return 0;
            else u = e[path.back() ^ 1].to, path.pop_back(), it[u]++;
        }
    }
    ll maxflow(int s, int t) {
        ll f = 0;
        while (bfs(s, t)) {
            it.assign(sz(g), 0);
            while (ll d = augment(s, t)) f += d;
        }
        return f;
    }
    ll flow(int id) const { return cap0[id] - e[id].cap; }  // < 0: reverse
    bool cut_side(int v) const { return lvl[v] >= 0; }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_dinic() {
    Dinic d(4);
    int a = d.add_edge(0, 1, 4);
    d.add_edge(0, 2, 2), d.add_edge(1, 2, 5);
    d.add_edge(1, 3, 2), d.add_edge(2, 3, 3);  // min cut: the 2 sink edges
    ll f = d.maxflow(0, 3);
    cout << f << ' ' << d.flow(a) << ' '  // 5 3 10
         << d.cut_side(1) << d.cut_side(3) << '\n';
}
