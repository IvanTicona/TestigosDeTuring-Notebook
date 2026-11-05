// Stress test for src/flows/global_min_cut.cpp
// Oracle: every bipartition into two non-empty sides (n <= 10); the
// returned side must be non-empty, proper, and have exactly the reported
// crossing weight. Zero-weight / disconnected graphs included.
#include "tests/stress/template_lib.h"
#include "src/flows/global_min_cut.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_global_min_cut, "src/flows/global_min_cut.cpp", "example_global_min_cut");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(2, 10), m = stress::rnd(0, 25);
        ll C = stress::rnd(0, 1) ? 5 : (ll)1e12;
        vector<vl> W(n, vl(n, 0));
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (u == v) continue;
            ll c = stress::rnd(0, C);
            W[u][v] += c, W[v][u] += c;
        }
        auto cut = [&](int mask) {
            ll c = 0;
            for (int u = 0; u < n; u++)
                for (int v = u + 1; v < n; v++)
                    if ((mask >> u & 1) != (mask >> v & 1)) c += W[u][v];
            return c;
        };
        ll best = INFL;
        for (int mask = 1; mask < (1 << n) - 1; mask++) best = min(best, cut(mask));
        auto [w, side] = global_min_cut(W);
        CHECK(w == best, "min cut n=" + to_string(n));
        int mask = 0;
        for (int v : side) mask |= 1 << v;
        CHECK(mask != 0 && mask != (1 << n) - 1 && cut(mask) == w, "side");
    }
    puts("OK");
}
