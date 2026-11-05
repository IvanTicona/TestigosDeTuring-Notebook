/**
 * Usage: SCC s(g);  s.ncomp, s.comp[v] in [0, ncomp)
 *        component ids are in TOPOLOGICAL order: an edge u -> v of the
 *        condensation has comp[u] < comp[v]
 *        s.dag(g) -> condensation adjacency (no duplicate edges)
 * Complexity: O(n + m), iterative (no recursion depth limit)
 * Source: UMSS Strongly Connected Components (recursive Tarjan, low = inf
 *         trick) + UPB SCC + UPB isc graph_scc + UTP tarjan / kosaraju;
 *         iterative version and topological numbering: own
 * Note: a node is in a cycle iff its component has size > 1 or a self
 *       loop. Min edges to make the graph strongly connected: with c > 1
 *       components, max(#sources, #sinks) of the condensation.
 * Status: ok
 */
struct SCC {
    int ncomp = 0;
    vi comp;
    SCC(const vector<vi> &g) {
        int n = sz(g), timer = 0;
        vi tin(n, -1), low(n), it(n, 0), st, cs;
        vector<char> on(n, 0);
        comp.assign(n, -1);
        for (int s = 0; s < n; s++) {
            if (tin[s] >= 0) continue;
            tin[s] = low[s] = timer++, st.pb(s), on[s] = 1, cs.pb(s);
            while (!cs.empty()) {
                int u = cs.back();
                if (it[u] < sz(g[u])) {
                    int v = g[u][it[u]++];
                    if (tin[v] < 0) {
                        tin[v] = low[v] = timer++;
                        st.pb(v), on[v] = 1, cs.pb(v);
                    } else if (on[v]) low[u] = min(low[u], tin[v]);
                    continue;
                }
                cs.pop_back();
                if (!cs.empty()) low[cs.back()] = min(low[cs.back()], low[u]);
                if (low[u] == tin[u]) {  // u is the root of a component
                    int v;
                    do v = st.back(), st.pop_back(), on[v] = 0, comp[v] = ncomp;
                    while (v != u);
                    ncomp++;
                }
            }
        }
        for (int &c : comp) c = ncomp - 1 - c;  // Tarjan finds sinks first
    }
    vector<vi> dag(const vector<vi> &g) const {
        vector<vi> d(ncomp);
        for (int u = 0; u < sz(g); u++)
            for (int v : g[u]) if (comp[u] != comp[v]) d[comp[u]].pb(comp[v]);
        for (auto &l : d) sort(all(l)), l.erase(unique(all(l)), l.end());
        return d;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_scc() {
    vector<vi> g = {{1}, {2}, {0, 3}, {4}, {3}, {}};  // {0,1,2} -> {3,4}, {5}
    SCC s(g);
    cout << s.ncomp << ' ' << (s.comp[0] == s.comp[2]) << ' '  // 3 1 1
         << (s.comp[0] < s.comp[3]) << '\n';
    cout << sz(s.dag(g)[s.comp[1]]) << '\n';  // 1
}
