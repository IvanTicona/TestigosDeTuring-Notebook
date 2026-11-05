/**
 * Usage: directed_mst(n, root, E) -> min total weight of an arborescence
 *        rooted at root (every node reachable from root by tree edges);
 *        -1 if some node cannot be reached. E[i] = {u, v, w}: edge u -> v
 * Complexity: O(n m) (Chu-Liu / Edmonds with contraction)
 * Source: UTP directed mst; own implementation (classic contraction)
 * Note: weights may be negative. Unknown root: add a super root with edges
 *       of weight BIG to every node, answer - BIG (if answer < 2 BIG the
 *       root is the node whose super edge was used). Reconstructing the
 *       edges needs the O(m log n) version (KACTL, rollback DSU + heaps).
 * Status: untested
 */
ll directed_mst(int n, int root, vector<array<ll, 3>> E) {
    ll res = 0;
    while (true) {
        vl in(n, INFL);
        vi pre(n, -1);
        for (auto [u, v, w] : E)
            if (u != v && w < in[v]) in[v] = w, pre[v] = u;
        for (int v = 0; v < n; v++) if (v != root && in[v] == INFL) return -1;
        in[root] = 0;
        int cnt = 0;
        vi id(n, -1), vis(n, -1);
        for (int v = 0; v < n; v++) {
            res += in[v];
            int x = v;
            while (vis[x] != v && id[x] == -1 && x != root)
                vis[x] = v, x = pre[x];
            if (x != root && id[x] == -1) {  // x is on a new cycle
                for (int y = pre[x]; y != x; y = pre[y]) id[y] = cnt;
                id[x] = cnt++;
            }
        }
        if (!cnt) return res;
        for (int v = 0; v < n; v++) if (id[v] == -1) id[v] = cnt++;
        vector<array<ll, 3>> F;  // contract cycles, reweight entering edges
        for (auto [u, v, w] : E)
            if (id[u] != id[v]) F.pb({id[u], id[v], w - in[v]});
        E = F, n = cnt, root = id[root];
    }
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_directed_mst() {
    vector<array<ll, 3>> E = {{0, 1, 10}, {0, 2, 2}, {1, 2, 1},
                              {2, 1, 3}, {1, 3, 4}, {2, 3, 8}};
    // best: 0->2, 2->1, 1->3 = 2 + 3 + 4
    cout << directed_mst(4, 0, E) << '\n';  // 9
}
