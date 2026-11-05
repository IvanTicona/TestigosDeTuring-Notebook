/**
 * Usage: auto [w, side] = global_min_cut(W);  W = n x n symmetric weights
 *        (n >= 2, parallel edges summed); w = min total weight of edges
 *        crossing a partition into two non-empty sides; side = vertices
 *        of one side
 * Complexity: O(n^3)
 * Source: UPB MinCut / Stoer-Wagner (int weights, INF 1e9) + UMSS Min Cut
 *         (s-t cut via flow, unit capacities only); ll + side recovery: own
 * Note: s-t min cut in a directed graph: max flow (dinic.cpp). Undirected
 *       all-pairs min cuts: gomory_hu.cpp.
 * Status: untested
 */
pair<ll, vi> global_min_cut(vector<vl> W) {
    int n = sz(W);
    vector<vi> grp(n);  // original vertices merged into each vertex
    for (int i = 0; i < n; i++) grp[i] = {i};
    vector<char> dead(n, 0);
    pair<ll, vi> best = {INFL, {}};
    for (int ph = n - 1; ph > 0; ph--) {
        vl w(n, 0);
        vector<char> in(n, 0);
        int prev = -1, last = -1;
        for (int k = 0; k <= ph; k++) {  // maximum adjacency order
            int sel = -1;
            for (int v = 0; v < n; v++)
                if (!dead[v] && !in[v] && (sel < 0 || w[v] > w[sel])) sel = v;
            in[sel] = 1, prev = last, last = sel;
            if (k == ph) {  // cut of the phase: last vs everything else
                if (w[last] < best.F) best = {w[last], grp[last]};
                grp[prev].insert(grp[prev].end(), all(grp[last]));
                for (int v = 0; v < n; v++)  // merge last into prev
                    W[prev][v] = W[v][prev] += W[last][v];
                dead[last] = 1;
            } else {
                for (int v = 0; v < n; v++) w[v] += W[sel][v];
            }
        }
    }
    return best;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_global_min_cut() {
    // two triangles (weights 3) joined by an edge of weight 1
    vector<vl> W(6, vl(6, 0));
    auto add = [&](int u, int v, ll c) { W[u][v] += c, W[v][u] += c; };
    add(0, 1, 3), add(1, 2, 3), add(2, 0, 3);
    add(3, 4, 3), add(4, 5, 3), add(5, 3, 3);
    add(2, 3, 1);
    auto [w, side] = global_min_cut(W);
    cout << w << ' ' << sz(side) << '\n';  // 1 3
}
