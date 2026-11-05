/**
 * Usage: ans = reroot<T>(g, id, merge, fin, lift)  (g = tree adjacency)
 *        sub(v)  = fin(merge of lift(sub(c), c, v) over children c, v)
 *        ans[v]  = the same with v as the root (all neighbors are children)
 *        merge: associative, id its identity; lift(x, c, p) turns the
 *        value of c's side into a contribution to its neighbor p.
 * Complexity: O(n) calls (prefix / suffix merges), iterative (no
 *             recursion depth issues on paths of 2e5 nodes)
 * Source: UPB RerootingDP (recursive, fixed sum example) + own
 *         (lambda interface, iterative)
 * Note: sum of distances: T = {size, sum}, fin(a) = {a.size + 1, a.sum},
 *       lift(x) = {x.size, x.sum + x.size}. Eccentricity: T = ll, merge
 *       max, lift = x + 1. Edge weights: lift(x, c, p) can read w(c, p).
 * Status: ok
 */
template <class T, class M, class Fn, class Lf>
vector<T> reroot(const vector<vi> &g, T id, M merge, Fn fin, Lf lift) {
    int n = sz(g);
    vi ord, par(n, -1);
    ord.reserve(n);
    ord.pb(0);
    for (int i = 0; i < sz(ord); i++)  // BFS order from root 0
        for (int u : g[ord[i]])
            if (u != par[ord[i]]) par[u] = ord[i], ord.pb(u);
    vector<T> down(n, id), up(n, id), ans(n, id);
    for (int i = n - 1; i >= 0; i--) {
        int v = ord[i];
        T acc = id;
        for (int c : g[v])
            if (c != par[v]) acc = merge(acc, lift(down[c], c, v));
        down[v] = fin(acc, v);
    }
    auto con = [&](int v, int u) {  // contribution of neighbor u to v
        return u == par[v] ? lift(up[v], u, v) : lift(down[u], u, v);
    };
    for (int v : ord) {
        int k = sz(g[v]);
        vector<T> pre(k + 1, id), suf(k + 1, id);
        for (int i = 0; i < k; i++) pre[i + 1] = merge(pre[i], con(v, g[v][i]));
        for (int i = k - 1; i >= 0; i--)
            suf[i] = merge(con(v, g[v][i]), suf[i + 1]);
        ans[v] = fin(pre[k], v);
        for (int i = 0; i < k; i++)  // c's view of v: v without c
            if (int c = g[v][i]; c != par[v])
                up[c] = fin(merge(pre[i], suf[i + 1]), v);
    }
    return ans;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_rerooting() {
    vector<vi> g = {{1}, {0, 2, 3}, {1}, {1, 4}, {3}};  // 0-1-2, 1-3-4
    auto sum = reroot<pll>(g, {0, 0},
        [](pll a, pll b) { return pll{a.F + b.F, a.S + b.S}; },
        [](pll a, int) { return pll{a.F + 1, a.S}; },
        [](pll x, int, int) { return pll{x.F, x.S + x.F}; });
    // sum of distances from each node
    cout << sum[0].S << ' ' << sum[1].S << ' ' << sum[4].S << '\n';  // 8 5 9
    auto ecc = reroot<ll>(g, 0, [](ll a, ll b) { return max(a, b); },
        [](ll a, int) { return a; }, [](ll x, int, int) { return x + 1; });
    cout << ecc[0] << ' ' << ecc[1] << ' ' << ecc[3] << '\n';  // 3 2 2
}
