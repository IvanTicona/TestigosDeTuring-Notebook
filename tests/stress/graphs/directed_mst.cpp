// Stress test for src/graphs/directed_mst.cpp
// Oracle: choose one incoming edge for every non-root node in all possible
// ways (n <= 6, few edges per node), keep the choices where every node
// reaches the root by following parents (no cycle), take the minimum.
#include "tests/stress/template_lib.h"
#include "src/graphs/directed_mst.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_directed_mst, "src/graphs/directed_mst.cpp", "example_directed_mst");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(1, 6), m = stress::rnd(0, 14), root = stress::rnd(0, n - 1);
        vector<array<ll, 3>> E(m);
        for (auto &e : E) e = {stress::rnd(0, n - 1), stress::rnd(0, n - 1), stress::rnd(-5, 20)};
        vector<vi> inc(n);
        for (int i = 0; i < m; i++) if (E[i][0] != E[i][1]) inc[E[i][1]].pb(i);
        ll best = INFL;
        vi choice(n, -1);
        auto rec = [&](auto &&self, int v) -> void {
            if (v == n) {
                for (int s = 0; s < n; s++) {  // follow parents to the root
                    int x = s, steps = 0;
                    while (x != root && steps <= n) x = E[choice[x]][0], steps++;
                    if (x != root) return;
                }
                ll c = 0;
                for (int u = 0; u < n; u++) if (u != root) c += E[choice[u]][2];
                best = min(best, c);
                return;
            }
            if (v == root) { self(self, v + 1); return; }
            for (int i : inc[v]) choice[v] = i, self(self, v + 1);
        };
        rec(rec, 0);
        CHECK(directed_mst(n, root, E) == (best == INFL ? -1 : best), "n=" + to_string(n) + " root=" + to_string(root));
    }
    puts("OK");
}
