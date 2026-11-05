/**
 * Usage: LowLink ll_(n, edges);  undirected, edges[i] = {u, v}
 *          (multi-edges and self-loops allowed)
 *        ll_.bridge[i]  edge i is a bridge;  ll_.cut[v]  articulation point
 *        ll_.ecomp[v]   2-edge-connected component id of v (bridges cut)
 *        ll_.bcc        vertex-biconnected components as lists of EDGE ids
 *                       (isolated vertices / self-loops belong to none)
 * Complexity: O(n + m), iterative DFS
 * Source: UPB BridgesArticulation_BCC (edge comp declared, never filled) +
 *         UPB isc graph_bcc + UTP bridges; iterative + edge stack: own
 * Note: block-cut tree: node per BCC + node per cut vertex, edge if the cut
 *       vertex is in the BCC (paths between vertices -> cut vertices
 *       crossed). Bridge tree: contract ecomp, bridges become tree edges.
 * Status: untested
 */
struct LowLink {
    vector<char> bridge, cut;
    vi ecomp;
    vector<vi> bcc;
    LowLink(int n, const vector<pii> &E)
        : bridge(sz(E), 0), cut(n, 0), ecomp(n, -1) {
        vector<vector<pii>> g(n);  // (to, edge id)
        for (int i = 0; i < sz(E); i++)
            g[E[i].F].pb({E[i].S, i}), g[E[i].S].pb({E[i].F, i});
        vi tin(n, -1), low(n), it(n, 0), es;
        int timer = 0;
        for (int s = 0; s < n; s++) {
            if (tin[s] >= 0) continue;
            vector<pii> cs = {{s, -1}};  // (vertex, edge to its parent)
            tin[s] = low[s] = timer++;
            int kids = 0;
            while (!cs.empty()) {
                auto [v, pe] = cs.back();
                if (it[v] < sz(g[v])) {
                    auto [to, id] = g[v][it[v]++];
                    if (id == pe) continue;
                    if (tin[to] < 0) {
                        es.pb(id), tin[to] = low[to] = timer++, cs.pb({to, id});
                    } else if (tin[to] < tin[v]) {
                        low[v] = min(low[v], tin[to]), es.pb(id);
                    }
                    continue;
                }
                cs.pop_back();
                if (cs.empty()) break;
                int p = cs.back().F;
                low[p] = min(low[p], low[v]);
                if (low[v] > tin[p]) bridge[pe] = 1;
                if (low[v] >= tin[p]) {  // p separates v's subtree
                    if (p != s) cut[p] = 1;
                    kids += p == s;
                    vi comp;
                    int e;
                    do e = es.back(), es.pop_back(), comp.pb(e);
                    while (e != pe);
                    bcc.pb(comp);
                }
            }
            if (kids > 1) cut[s] = 1;
        }
        int c = 0;  // 2-edge-connected components: flood fill without bridges
        for (int s = 0; s < n; s++) {
            if (ecomp[s] >= 0) continue;
            vi st = {s};
            ecomp[s] = c;
            while (!st.empty()) {
                int v = st.back();
                st.pop_back();
                for (auto [to, id] : g[v])
                    if (!bridge[id] && ecomp[to] < 0) ecomp[to] = c, st.pb(to);
            }
            c++;
        }
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_bridges() {
    // triangle 0-1-2, bridge 2-3, triangle 3-4-5
    vector<pii> E = {{0, 1}, {1, 2}, {2, 0}, {2, 3}, {3, 4}, {4, 5}, {5, 3}};
    LowLink L(6, E);
    // edge 3 is a bridge, edge 0 not; vertex 2 is a cut vertex, 0 not
    int b3 = L.bridge[3], b0 = L.bridge[0], c2 = L.cut[2], c0 = L.cut[0];
    cout << b3 << b0 << ' ' << c2 << c0 << '\n';  // 10 10
    bool same = L.ecomp[0] == L.ecomp[2], diff = L.ecomp[2] != L.ecomp[3];
    cout << sz(L.bcc) << ' ' << same << diff << '\n';  // 3 11
}
