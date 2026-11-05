// Stress test for src/graphs/bellman_ford.cpp
// Oracles: Floyd-Warshall with -inf marking (via k with d[k][k] < 0) for
// single-source distances; cycle existence by Floyd (some d[i][i] < 0); the
// returned cycle must use existing edges and have negative total weight.
#include "tests/stress/template_lib.h"
#include "src/graphs/bellman_ford.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_bellman_ford, "src/graphs/bellman_ford.cpp", "example_bellman_ford");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 10), m = stress::rnd(0, 25);
        vector<array<ll, 3>> E;
        vector<vl> f(n, vl(n, INFL));
        for (int i = 0; i < n; i++) f[i][i] = 0;
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), w = stress::rnd(-4, 20);
            E.pb({u, v, w});
            f[u][v] = min(f[u][v], w);
        }
        for (int k = 0; k < n; k++)
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    if (f[i][k] < INFL && f[k][j] < INFL) f[i][j] = min(f[i][j], f[i][k] + f[k][j]);
        bool has = false;
        for (int i = 0; i < n; i++) has |= f[i][i] < 0;
        int s = stress::rnd(0, n - 1);
        vl d = bellman_ford(n, E, s);
        for (int v = 0; v < n; v++) {
            bool minf = false;
            for (int k = 0; k < n; k++) minf |= f[s][k] < INFL && f[k][k] < 0 && f[k][v] < INFL;
            ll want = minf ? -INFL : f[s][v];
            CHECK(d[v] == want, "dist v=" + to_string(v));
        }
        vi c = neg_cycle(n, E);
        CHECK(c.empty() == !has, "cycle existence");
        if (!c.empty()) {
            ll tot = 0;
            for (int i = 0; i < sz(c); i++) {
                int u = c[i], v = c[(i + 1) % sz(c)];
                ll best = INFL;
                for (auto [a, b, w] : E) if (a == u && b == v) best = min(best, w);
                CHECK(best < INFL, "cycle edge exists");
                tot += best;
            }
            CHECK(tot < 0, "cycle is negative");
        }
    }
    puts("OK");
}
