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
    // huge weights (|w| n < INFL but many relaxations): the old version
    // overflowed ll and lost the -INFL mark. Oracle: Floyd in i128.
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 8), m = stress::rnd(0, 25);
        const ll W = (ll)1e17;
        vector<array<ll, 3>> E;
        const i128 BIG = (i128)1e30;
        vector<vector<i128>> f(n, vector<i128>(n, BIG));
        for (int i = 0; i < n; i++) f[i][i] = 0;
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), w = stress::rnd(-W, W);
            E.pb({u, v, w});
            f[u][v] = min(f[u][v], (i128)w);
        }
        for (int k = 0; k < n; k++)
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    if (f[i][k] < BIG && f[k][j] < BIG) f[i][j] = max(-BIG, min(f[i][j], f[i][k] + f[k][j]));
        bool has = false;
        for (int i = 0; i < n; i++) has |= f[i][i] < 0;
        int s = stress::rnd(0, n - 1);
        vl d = bellman_ford(n, E, s);
        for (int v = 0; v < n; v++) {
            bool minf = false;
            for (int k = 0; k < n; k++) minf |= f[s][k] < BIG && f[k][k] < 0 && f[k][v] < BIG;
            i128 want = minf ? -INFL : f[s][v] < BIG ? f[s][v] : (i128)INFL;
            CHECK(d[v] == want, "big dist v=" + to_string(v));
        }
        vi c = neg_cycle(n, E);
        CHECK(c.empty() == !has, "big cycle existence");
        if (!c.empty()) {
            i128 tot = 0;
            for (int i = 0; i < sz(c); i++) {
                int u = c[i], v = c[(i + 1) % sz(c)];
                ll best = INFL;
                for (auto [a, b, w] : E) if (a == u && b == v) best = min(best, w);
                CHECK(best < INFL, "big cycle edge exists");
                tot += best;
            }
            CHECK(tot < 0, "big cycle is negative");
        }
    }
    {  // long negative cycle 0 -> 1 -> ... -> n-1 -> 0, plus a tail node
        int n = 3000;  // |w| n = 9e17 < INFL, but n^2 |w| overflows ll
        vector<array<ll, 3>> E;
        for (int i = 0; i < n; i++) E.pb({i, (i + 1) % n, -(ll)3e14});
        E.pb({5, n, 1});
        vl d = bellman_ford(n + 1, E, 0);
        CHECK(d[5] == -INFL && d[n] == -INFL, "long cycle -INFL");
        CHECK(sz(neg_cycle(n + 1, E)) == n, "long cycle found");
    }
    puts("OK");
}
