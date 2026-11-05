// Stress test for src/trees/lca.cpp
// Oracle: explicit parent pointers walked step by step (naive LCA by
// lifting the deeper node then both), path enumeration for dist / max /
// k-th vertex; random trees of many shapes (paths, stars, random) plus a
// 2e5-node path for the iterative build.
#include "tests/stress/template_lib.h"
#include "src/trees/lca.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_lca, "src/trees/lca.cpp", "example_lca");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 60), shape = stress::rnd(0, 2), root = stress::rnd(0, n - 1);
        vector<vector<pll>> g(n);
        vi par(n, -1);
        vl pw(n, 0);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        for (int i = 1; i < n; i++) {
            int p = shape == 0 ? i - 1 : shape == 1 ? 0 : stress::rnd(0, i - 1);
            ll w = stress::rnd(-(ll)1e9, (ll)1e9);
            int a = perm[i], b = perm[p];
            g[a].pb({b, w}), g[b].pb({a, w});
        }
        // naive rooting at root
        vi dep(n, 0), order = {root};
        vector<char> seen(n, 0);
        seen[root] = 1;
        for (int i = 0; i < sz(order); i++)
            for (auto [v, w] : g[order[i]])
                if (!seen[v]) seen[v] = 1, par[v] = order[i], pw[v] = w, dep[v] = dep[order[i]] + 1, order.pb(v);
        LCA t(g, root);
        for (int q = 0; q < 100; q++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            vi pu, pv;  // paths up to the lca
            int a = u, b = v;
            vl wa, wb;
            while (dep[a] > dep[b]) pu.pb(a), wa.pb(pw[a]), a = par[a];
            while (dep[b] > dep[a]) pv.pb(b), wb.pb(pw[b]), b = par[b];
            while (a != b) pu.pb(a), wa.pb(pw[a]), a = par[a], pv.pb(b), wb.pb(pw[b]), b = par[b];
            CHECK(t.lca(u, v) == a, "lca");
            ll d = 0, mxw = LLONG_MIN;
            for (ll x : wa) d += x, mxw = max(mxw, x);
            for (ll x : wb) d += x, mxw = max(mxw, x);
            CHECK(t.dist(u, v) == d && t.path_max(u, v) == mxw, "dist / path_max");
            vi path = pu;
            path.pb(a);
            for (int i = sz(pv) - 1; i >= 0; i--) path.pb(pv[i]);
            int k = stress::rnd(0, sz(path));
            CHECK(t.kth_on_path(u, v, k) == (k < sz(path) ? path[k] : -1), "kth_on_path");
            int kk = stress::rnd(0, n);
            int x = u;
            for (int s = 0; s < kk && x >= 0; s++) x = par[x];
            CHECK(t.kth_anc(u, kk) == x, "kth_anc");
        }
    }
    int n = 200000;
    vector<vector<pll>> g(n);
    for (int i = 1; i < n; i++) g[i - 1].pb({i, 1}), g[i].pb({i - 1, 1});
    LCA t(g, 0);
    CHECK(t.lca(n - 1, n / 2) == n / 2 && t.dist(0, n - 1) == n - 1, "big path");
    puts("OK");
}
