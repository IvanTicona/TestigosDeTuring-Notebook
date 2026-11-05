/**
 * Usage: Centroid cd(g);  g = tree adjacency
 *        cd.cpar[v] = parent of v in the centroid tree (-1 for the top)
 *        cd.anc[v]  = {(c, dist(v, c))} for every centroid ancestor c of v
 *                     (including v itself), from the top down
 * Complexity: O(n log n) time and memory; iterative (no recursion)
 * Source: UMSS Centroid Descomposition + UMSS Online Centroid (nearest
 *         red node pattern) + UPB CentroidDecomposition + UPB isc
 *         ds_centroid_decomposition; distances per level: own
 * Note: every u - v path passes through the deepest common centroid
 *       ancestor c: dist(u, v) = d(u, c) + d(v, c). Nearest marked node:
 *       best[c] = min d(x, c) over marked x in c's part; query v = min over
 *       (c, d) in anc[v] of best[c] + d. Counting paths with a property:
 *       solve at c, subtract pairs inside the same child part.
 * Status: untested
 */
struct Centroid {
    vi cpar;
    vector<vector<pii>> anc;
    Centroid(const vector<vi> &g) {
        int n = sz(g);
        cpar.assign(n, -1), anc.assign(n, {});
        vector<char> rem(n, 0);
        vi sub(n), par(n);
        vector<pii> todo = {{0, -1}};  // (any vertex of a part, its parent)
        while (!todo.empty()) {
            auto [r, pc] = todo.back();
            todo.pop_back();
            vi ord = {r};  // BFS over the current part
            par[r] = -1;
            for (int i = 0; i < sz(ord); i++)
                for (int v : g[ord[i]])
                    if (!rem[v] && v != par[ord[i]]) par[v] = ord[i], ord.pb(v);
            for (int i = sz(ord) - 1; i >= 0; i--) {
                sub[ord[i]] = 1;
                for (int v : g[ord[i]])
                    if (!rem[v] && v != par[ord[i]]) sub[ord[i]] += sub[v];
            }
            int tot = sz(ord), c = r;
            while (true) {  // walk towards the heavy child
                int nx = -1;
                for (int v : g[c])
                    if (!rem[v] && v != par[c] && 2 * sub[v] > tot) nx = v;
                if (nx < 0) break;
                c = nx;
            }
            cpar[c] = pc;
            vector<pii> q = {{c, 0}};  // distances from the centroid
            vi from = {-1};
            for (int i = 0; i < sz(q); i++) {
                auto [u, d] = q[i];
                anc[u].pb({c, d});
                for (int v : g[u])
                    if (!rem[v] && v != from[i]) q.pb({v, d + 1}), from.pb(u);
            }
            rem[c] = 1;
            for (int v : g[c]) if (!rem[v]) todo.pb({v, c});
        }
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_centroid() {
    vector<vi> g = {{1}, {0, 2}, {1, 3}, {2, 4}, {3}};  // path 0-1-2-3-4
    Centroid cd(g);
    // centroid tree: 2 on top, then 1 and 3, then 0 and 4
    cout << cd.cpar[2] << ' ' << cd.cpar[0] << ' '  // -1 1 3
         << sz(cd.anc[0]) << '\n';
    auto [c, d] = cd.anc[4][0];  // top centroid and its distance to 4
    cout << c << ' ' << d << '\n';  // 2 2
}
