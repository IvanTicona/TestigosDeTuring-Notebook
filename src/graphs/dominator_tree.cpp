/**
 * Usage: vi idom = dominators(g, root);  directed graph
 *        idom[root] = root, idom[v] = immediate dominator of v (the last
 *        node that EVERY path root -> v must cross), -1 if unreachable
 * Complexity: O((n + m) log n), iterative (no recursion)
 * Source: own (Lengauer-Tarjan, semi-dominators + DSU with path
 *         compression); missing in all three references
 * Note: d dominates v iff v is unreachable from root once d is removed.
 *       Edges whose removal disconnects (bridges of a flow graph): split
 *       each edge with a middle node. Undirected articulation points
 *       relative to a root: bridges.cpp instead.
 * Status: untested
 */
vi dominators(const vector<vi> &g, int root) {
    int n = sz(g), cnt = 0;
    vi ord(n, -1), rev(n), par(n), it(n, 0);
    vector<vi> rg(n), bucket(n);
    vi st = {root};  // iterative DFS: preorder numbering
    ord[root] = cnt, rev[cnt++] = root;
    while (!st.empty()) {
        int u = st.back();
        if (it[u] == sz(g[u])) { st.pop_back(); continue; }
        int v = g[u][it[u]++];
        if (ord[v] < 0)
            ord[v] = cnt, rev[cnt] = v, par[cnt++] = ord[u], st.pb(v);
    }
    for (int u = 0; u < n; u++)  // predecessors, in preorder indices
        if (ord[u] >= 0)
            for (int v : g[u]) if (ord[v] >= 0) rg[ord[v]].pb(ord[u]);
    vi sdom(cnt), dom(cnt), dsu(cnt), label(cnt);
    iota(all(sdom), 0), iota(all(dsu), 0), iota(all(label), 0);
    auto find = [&](int u) {  // min-sdom label on u's path, excluding root
        if (dsu[u] == u) return u;
        vi path;
        int x = u;
        while (dsu[dsu[x]] != dsu[x]) path.pb(x), x = dsu[x];
        for (int k = sz(path) - 1; k >= 0; k--) {
            int y = path[k];
            if (sdom[label[dsu[y]]] < sdom[label[y]]) label[y] = label[dsu[y]];
            dsu[y] = x;
        }
        return label[u];
    };
    for (int i = cnt - 1; i >= 0; i--) {
        for (int j : rg[i]) sdom[i] = min(sdom[i], sdom[find(j)]);
        if (i > 0) bucket[sdom[i]].pb(i);
        for (int w : bucket[i]) {
            int v = find(w);
            dom[w] = sdom[v] == sdom[w] ? sdom[w] : v;
        }
        if (i > 0) dsu[i] = par[i];
    }
    for (int i = 1; i < cnt; i++) if (dom[i] != sdom[i]) dom[i] = dom[dom[i]];
    vi idom(n, -1);
    for (int i = 0; i < cnt; i++) idom[rev[i]] = rev[dom[i]];
    return idom;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_dominator_tree() {
    // 0 -> 1 -> 2 -> 4, 0 -> 3 -> 2: node 2 is reached by two ways
    vector<vi> g = {{1, 3}, {2}, {4}, {2}, {}, {0}};
    vi d = dominators(g, 0);
    cout << d[0] << d[1] << d[2] << d[3] << d[4] << ' '  // 00002 -1
         << d[5] << '\n';
}
