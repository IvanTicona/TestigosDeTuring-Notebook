/**
 * Usage: sack(g, root, add, rem, ans): add(x) / rem(x) put / take vertex x
 *        into / out of your structure; ans(v) is called exactly when the
 *        structure holds the subtree of v (and nothing else)
 * Complexity: O(n log n) add / rem calls; iterative (no recursion)
 * Source: UPB DSU_On_Tree_Sack (big[v] == 0 used as "no heavy child":
 *         breaks when vertex 0 is a child; removal never undid maxf);
 *         generic lambdas + explicit stack: own
 * Note: "small to large": keep the heavy child's data, re-add light
 *       subtrees. Typical: #distinct colors / most frequent color / count
 *       of nodes at depth d in each subtree. Alternative: merge sets small
 *       into large (O(n log^2 n)). Forests: every tree is solved (root's
 *       tree first); the structure is empty when a new tree starts.
 * Status: ok
 */
template <class A, class R, class Q>
void sack(const vector<vi> &g, int root, A add, R rem, Q ans) {
    int n = sz(g);
    vi par(n, -1), ord, sub(n, 1), big(n, -1), tin(n), tout(n), ver, roots;
    vector<char> seen(n, 0);
    for (int k = -1; k < n; k++) {  // forest: root first, then other trees
        int r = k < 0 ? root : k;
        if (r >= n || seen[r]) continue;
        seen[r] = 1, roots.pb(r), ord.pb(r);
        for (int i = sz(ord) - 1; i < sz(ord); i++)
            for (int v : g[ord[i]])
                if (!seen[v]) seen[v] = 1, par[v] = ord[i], ord.pb(v);
    }
    for (int i = n - 1; i >= 0; i--)
        if (par[ord[i]] >= 0) sub[par[ord[i]]] += sub[ord[i]];
    for (int v : ord)
        for (int c : g[v])
            if (c != par[v] && (big[v] < 0 || sub[c] > sub[big[v]])) big[v] = c;
    for (int r : roots) {  // preorder: subtree of v = ver[tin[v] .. tout[v]]
        vi st = {r};
        while (!st.empty()) {
            int v = st.back();
            st.pop_back();
            tin[v] = sz(ver), tout[v] = tin[v] + sub[v] - 1, ver.pb(v);
            for (int c : g[v]) if (c != par[v]) st.pb(c);
        }
    }
    struct F { int v, i; bool keep, heavy_done; };
    for (int r : roots) {  // keep = false: empty between trees
        vector<F> s = {{r, 0, false, false}};
        while (!s.empty()) {
            int v = s.back().v;
            if (!s.back().heavy_done) {
                int &i = s.back().i;  // next light child to solve (and drop)
                while (i < sz(g[v]) && (g[v][i] == par[v] || g[v][i] == big[v]))
                    i++;
                if (i < sz(g[v])) {
                    int c = g[v][i++];
                    s.pb({c, 0, false, false});
                    continue;
                }
                s.back().heavy_done = true;
                if (big[v] >= 0) { s.pb({big[v], 0, true, false}); continue; }
            }
            for (int c : g[v])
                if (c != par[v] && c != big[v])
                    for (int p = tin[c]; p <= tout[c]; p++) add(ver[p]);
            add(v), ans(v);
            bool keep = s.back().keep;
            s.pop_back();
            if (!keep)
                for (int p = tin[v]; p <= tout[v]; p++) rem(ver[p]);
        }
    }
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_sack() {
    vector<vi> g = {{1, 2}, {0, 3, 4}, {0}, {1}, {1}};
    vi col = {1, 2, 1, 2, 3}, cnt(4, 0), res(5);
    int distinct = 0;  // #distinct colors in each subtree
    sack(g, 0, [&](int x) { distinct += cnt[col[x]]++ == 0; },
         [&](int x) { distinct -= --cnt[col[x]] == 0; },
         [&](int v) { res[v] = distinct; });
    cout << res[0] << res[1] << res[2] << res[3] << res[4] << '\n';  // 32111
}
