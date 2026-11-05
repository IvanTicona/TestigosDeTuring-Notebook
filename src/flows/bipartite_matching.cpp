/**
 * Usage: HopcroftKarp hk(nl, nr); hk.add_edge(u, v) (u left, v right);
 *        int k = hk.solve();  hk.ml[u] / hk.mr[v] = partner or -1
 *        auto [L, R] = hk.cover() -> min vertex cover (|L| + |R| = k):
 *          König; max independent set = complement of the cover
 *        path_cover(n, E) -> min #vertex-disjoint paths covering a DAG
 * Complexity: O(E sqrt V)
 * Source: UPB HopcroftKarp (code/) + UPB isc graph_hopcroft_karp (dist
 *         not reset between phases) + UPB MaxBipartiteMatching (Kuhn) +
 *         UTP konig's theorem, minimum path cover; cover(): own
 * Note: König: max matching = min vertex cover (bipartite only). Min
 *       edge cover = V - max matching (no isolated vertices). Path cover
 *       allowing shared vertices: run path_cover on the transitive
 *       closure (= max antichain, Dilworth).
 * Status: untested
 */
struct HopcroftKarp {
    int nl, nr;
    vector<vi> g;
    vi ml, mr, dist, ip, st;
    HopcroftKarp(int nl_, int nr_)
        : nl(nl_), nr(nr_), g(nl_), ml(nl_, -1), mr(nr_, -1) {}
    void add_edge(int u, int v) { g[u].pb(v); }
    bool bfs() {
        dist.assign(nl, -1);
        queue<int> q;
        for (int u = 0; u < nl; u++) if (ml[u] < 0) dist[u] = 0, q.push(u);
        bool found = false;
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int v : g[u]) {
                int w = mr[v];
                if (w < 0) found = true;
                else if (dist[w] < 0) dist[w] = dist[u] + 1, q.push(w);
            }
        }
        return found;
    }
    bool dfs(int r) {  // iterative: alternating paths can be ~n long
        st = {r};
        while (!st.empty()) {
            int u = st.back();
            if (ip[u] == sz(g[u])) {  // dead end for this phase
                dist[u] = -1, st.pop_back();
                if (!st.empty()) ip[st.back()]++;
                continue;
            }
            int w = mr[g[u][ip[u]]];
            if (w < 0) {  // free right vertex: flip the whole path
                for (int x : st) ml[x] = g[x][ip[x]], mr[ml[x]] = x;
                return true;
            }
            if (dist[w] == dist[u] + 1) st.pb(w);
            else ip[u]++;
        }
        return false;
    }
    int solve() {
        int k = 0;
        while (bfs()) {
            ip.assign(nl, 0);
            for (int u = 0; u < nl; u++) if (ml[u] < 0 && dfs(u)) k++;
        }
        return k;
    }
    pair<vi, vi> cover() {  // call after solve()
        vector<char> vl_(nl, 0), vr(nr, 0);  // alternating reachability
        queue<int> q;
        for (int u = 0; u < nl; u++) if (ml[u] < 0) vl_[u] = 1, q.push(u);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (int v : g[u])
                if (!vr[v]) {
                    vr[v] = 1;
                    if (int w = mr[v]; w >= 0 && !vl_[w]) vl_[w] = 1, q.push(w);
                }
        }
        vi L, R;
        for (int u = 0; u < nl; u++) if (!vl_[u]) L.pb(u);
        for (int v = 0; v < nr; v++) if (vr[v]) R.pb(v);
        return {L, R};
    }
};

int path_cover(int n, const vector<pii> &E) {  // DAG, vertex-disjoint
    HopcroftKarp hk(n, n);
    for (auto [u, v] : E) hk.add_edge(u, v);
    return n - hk.solve();
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_bipartite_matching() {
    HopcroftKarp hk(3, 3);  // left 0 likes {0, 1}, 1 likes {0}, 2 likes {1, 2}
    hk.add_edge(0, 0), hk.add_edge(0, 1), hk.add_edge(1, 0);
    hk.add_edge(2, 1), hk.add_edge(2, 2);
    int k = hk.solve();
    auto [L, R] = hk.cover();
    cout << k << ' ' << hk.ml[1] << ' ' << sz(L) + sz(R) << '\n';  // 3 0 3
    cout << path_cover(4, {{0, 1}, {1, 2}, {0, 3}}) << '\n';  // 2
}
