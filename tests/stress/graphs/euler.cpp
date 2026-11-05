// Stress test for src/graphs/euler.cpp
// Oracle: DFS over all edge orders (m <= 8) deciding whether an Eulerian
// trail exists (optionally from src); the returned trail is validated edge
// by edge (each edge once, consecutive, orientation for directed graphs).
#include "tests/stress/template_lib.h"
#include "src/graphs/euler.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_euler, "src/graphs/euler.cpp", "example_euler");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(1, 6), m = stress::rnd(0, 8);
        bool dir = stress::rnd(0, 1);
        vector<pii> E(m);
        for (auto &[u, v] : E) u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
        int src = stress::rnd(0, 2) ? -1 : stress::rnd(0, n - 1);
        vector<char> used(m, 0);
        auto dfs = [&](auto &&self, int u, int left) -> bool {
            if (!left) return true;
            for (int i = 0; i < m; i++) {
                if (used[i]) continue;
                int v = -1;
                if (E[i].F == u) v = E[i].S;
                else if (!dir && E[i].S == u) v = E[i].F;
                if (v < 0) continue;
                used[i] = 1;
                bool ok = self(self, v, left - 1);
                used[i] = 0;
                if (ok) return true;
            }
            return false;
        };
        bool exists = false;
        for (int s = 0; s < n && !exists; s++)
            if (src < 0 || s == src) exists = dfs(dfs, s, m);
        if (m == 0) exists = true;
        auto [vs, es] = euler(n, E, dir, src);
        CHECK(vs.empty() == !exists, "existence n=" + to_string(n) + " m=" + to_string(m) + " dir=" + to_string(dir));
        if (vs.empty()) continue;
        CHECK(sz(vs) == m + 1 && sz(es) == m, "lengths");
        if (src >= 0) CHECK(vs[0] == src, "starts at src");
        CHECK(sz(set<int>(all(es))) == m, "each edge once");
        for (int i = 0; i < m; i++) {
            auto [a, b] = E[es[i]];
            bool fw = a == vs[i] && b == vs[i + 1], bw = b == vs[i] && a == vs[i + 1];
            CHECK(fw || (!dir && bw), "consecutive edge");
        }
    }
    puts("OK");
}
