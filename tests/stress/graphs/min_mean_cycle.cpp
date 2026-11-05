// Stress test for src/graphs/min_mean_cycle.cpp
// Oracle: enumerate every simple cycle (DFS from its smallest vertex,
// n <= 6, multi-edges and self-loops) and keep the minimum mean as an
// exact fraction; the minimum mean over all cycles is attained by a
// simple one.
#include "tests/stress/template_lib.h"
#include "src/graphs/min_mean_cycle.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_min_mean_cycle, "src/graphs/min_mean_cycle.cpp", "example_min_mean_cycle");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(1, 6), m = stress::rnd(0, 12);
        vector<array<ll, 3>> E(m);
        for (auto &e : E) e = {stress::rnd(0, n - 1), stress::rnd(0, n - 1), stress::rnd(-10, 10)};
        bool found = false;
        ll bp = 0, bq = 1;
        vector<char> on(n, 0);
        auto dfs = [&](auto &&self, int s, int u, ll sum, int len) -> void {
            for (auto [a, b, w] : E) {
                if (a != u || b < s) continue;
                if (b == s) {  // closed a simple cycle
                    ll p = sum + w, q = len + 1;
                    if (!found || (i128)p * bq < (i128)bp * q) bp = p, bq = q, found = true;
                } else if (!on[b]) {
                    on[b] = 1, self(self, s, b, sum + w, len + 1), on[b] = 0;
                }
            }
        };
        for (int s = 0; s < n; s++) on[s] = 1, dfs(dfs, s, s, 0, 0), on[s] = 0;
        pll want = {0, 0};
        if (found) {
            ll g = __gcd(llabs(bp), bq);
            want = {bp / g, bq / g};
        }
        CHECK(min_mean_cycle(n, E) == want, "n=" + to_string(n) + " m=" + to_string(m));
    }
    puts("OK");
}
