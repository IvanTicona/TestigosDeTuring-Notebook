// Stress test for src/graphs/bfs.cpp
// Oracles: Bellman-Ford style relaxation (n rounds over all edges) for BFS
// and 0-1 distances; returned paths re-validated edge by edge with the
// right length; bipartiteness by trying all 2^n colorings (n <= 12).
#include "tests/stress/template_lib.h"
#include "src/graphs/bfs.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_bfs, "src/graphs/bfs.cpp", "example_bfs");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 12), m = stress::rnd(0, 25);
        bool directed = stress::rnd(0, 1);
        vector<vi> g(n);
        vector<vector<pii>> h(n);
        vector<array<int, 3>> E;
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), w = stress::rnd(0, 1);
            g[u].pb(v), h[u].pb({v, w}), E.pb({u, v, w});
            if (!directed) g[v].pb(u), h[v].pb({u, w}), E.pb({v, u, w});
        }
        vi src;
        for (int k = stress::rnd(1, 3); k--;) src.pb(stress::rnd(0, n - 1));
        vl d1(n, INFL), d01(n, INFL);
        for (int s : src) d1[s] = 0;
        d01[src[0]] = 0;
        for (int r = 0; r < n; r++)
            for (auto [u, v, w] : E) {
                if (d1[u] < INFL) d1[v] = min(d1[v], d1[u] + 1);
                if (d01[u] < INFL) d01[v] = min(d01[v], d01[u] + w);
            }
        auto [d, par] = bfs(g, src);
        for (int v = 0; v < n; v++) {
            CHECK(d[v] == (d1[v] == INFL ? -1 : d1[v]), "bfs dist");
            vi p = get_path(par, v, d);
            if (d[v] < 0) { CHECK(p.empty(), "no path"); continue; }
            CHECK(sz(p) == d[v] + 1 && p.back() == v && count(all(src), p[0]), "path ends");
            for (int i = 0; i + 1 < sz(p); i++) CHECK(count(all(g[p[i]]), p[i + 1]), "path edge");
        }
        CHECK(bfs01(h, src[0]) == d01, "bfs01");
        if (!directed) {
            bool ok = false;
            for (int mask = 0; mask < (1 << n) && !ok; mask++) {
                bool good = true;
                for (auto [u, v, w] : E) good &= (mask >> u & 1) != (mask >> v & 1);
                ok |= good;
            }
            vi c = bipartite(g);
            CHECK(c.empty() == !ok, "bipartite existence");
            if (!c.empty()) for (auto [u, v, w] : E) CHECK(c[u] != c[v], "proper coloring");
        }
    }
    puts("OK");
}
