// Stress test for src/flows/gomory_hu.cpp
// Oracle: for every pair (u, v), the min u-v cut by enumerating all
// bipartitions (n <= 9) of the undirected graph must equal gh_min_cut;
// the tree must have n - 1 edges forming a tree rooted at 0.
#include "tests/stress/template_lib.h"
#include "src/flows/dinic.cpp"
#include "src/flows/gomory_hu.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_gomory_hu, "src/flows/gomory_hu.cpp", "example_gomory_hu");
    for (stress::iter = 0; stress::iter < stress::scaled(500); stress::iter++) {
        int n = stress::rnd(2, 9), m = stress::rnd(0, 18);
        vector<array<ll, 3>> E;
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (u != v) E.pb({u, v, stress::rnd(0, 10)});
        }
        vl cutw(1 << n, 0);
        for (int mask = 0; mask < (1 << n); mask++)
            for (auto [u, v, c] : E)
                if ((mask >> u & 1) != (mask >> v & 1)) cutw[mask] += c;
        auto t = gomory_hu(n, E);
        for (int v = 1; v < n; v++) {  // parents lead to 0 without cycles
            int x = v, steps = 0;
            while (x != 0 && steps <= n) x = t[x].F, steps++;
            CHECK(x == 0, "tree");
        }
        for (int u = 0; u < n; u++)
            for (int v = u + 1; v < n; v++) {
                ll best = INFL;
                for (int mask = 0; mask < (1 << n); mask++)
                    if ((mask >> u & 1) && !(mask >> v & 1)) best = min(best, cutw[mask]);
                CHECK(gh_min_cut(t, u, v) == best, "pair " + to_string(u) + " " + to_string(v));
            }
    }
    puts("OK");
}
