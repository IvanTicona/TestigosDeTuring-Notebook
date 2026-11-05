// Stress test for src/graphs/dijkstra.cpp
// Oracles: Bellman-Ford relaxation for distances (multi-source, weights up
// to 1e12, zero weights); parent pointers must form shortest paths; walk
// counting DP cnt[len][v] (weights >= 1) for the k shortest walk lengths.
#include "tests/stress/template_lib.h"
#include "src/graphs/dijkstra.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_dijkstra, "src/graphs/dijkstra.cpp", "example_dijkstra");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 15), m = stress::rnd(0, 40);
        vector<vector<pll>> g(n);
        vector<array<ll, 3>> E;
        ll W = stress::rnd(0, 1) ? 5 : (ll)1e12;
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            ll w = stress::rnd(0, W);
            g[u].pb({v, w}), E.pb({u, v, w});
        }
        vi src;
        for (int k = stress::rnd(1, 3); k--;) src.pb(stress::rnd(0, n - 1));
        vl bf(n, INFL);
        for (int s : src) bf[s] = 0;
        for (int r = 0; r < n; r++)
            for (auto [u, v, w] : E) if (bf[u] < INFL) bf[v] = min(bf[v], bf[u] + w);
        auto [d, par] = dijkstra(g, src);
        CHECK(d == bf, "distances");
        for (int v = 0; v < n; v++) {
            if (d[v] == INFL || par[v] < 0) continue;
            ll best = INFL;  // some edge par -> v must realize d[v]
            for (auto [x, w] : g[par[v]]) if (x == v) best = min(best, d[par[v]] + w);
            CHECK(best == d[v], "parent edge on a shortest path");
        }
    }
    for (stress::iter = 0; stress::iter < stress::scaled(800); stress::iter++) {
        int n = stress::rnd(1, 7), m = stress::rnd(0, 14), k = stress::rnd(1, 10);
        vector<vector<pll>> g(n);
        vector<array<ll, 3>> E;
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            ll w = stress::rnd(1, 5);
            g[u].pb({v, w}), E.pb({u, v, w});
        }
        int s = stress::rnd(0, n - 1), t = stress::rnd(0, n - 1);
        const int L = 60;
        vector<vl> cnt(L + 1, vl(n, 0));
        cnt[0][s] = 1;
        for (int len = 0; len < L; len++)
            for (auto [u, v, w] : E)
                if (len + w <= L) cnt[len + w][v] = min(cnt[len + w][v] + cnt[len][u], 100LL);
        vl want;
        for (int len = 0; len <= L && sz(want) < k; len++)
            for (ll c = 0; c < cnt[len][t] && sz(want) < k; c++) want.pb(len);
        vl got = k_shortest(g, s, t, k);
        CHECK(sz(got) >= sz(want), "k_shortest count");
        CHECK(vl(got.begin(), got.begin() + sz(want)) == want, "k_shortest lengths");
    }
    puts("OK");
}
