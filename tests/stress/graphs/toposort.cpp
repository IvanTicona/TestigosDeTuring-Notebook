// Stress test for src/graphs/toposort.cpp
// Oracles: all n! permutations (n <= 7) to decide acyclicity and the
// lexicographically smallest valid order; returned orders / cycles
// re-validated; longest path by memoized recursion over successors.
#include "tests/stress/template_lib.h"
#include "src/graphs/toposort.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_toposort, "src/graphs/toposort.cpp", "example_toposort");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(1, 7), m = stress::rnd(0, 14);
        bool dag = stress::rnd(0, 1);
        vector<vi> g(n);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        vi pos(n);
        for (int i = 0; i < n; i++) pos[perm[i]] = i;
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (dag && pos[u] >= pos[v]) continue;  // keep a hidden order
            g[u].pb(v);
        }
        vi p(n), best;
        iota(all(p), 0);
        do {
            vi ps(n);
            for (int i = 0; i < n; i++) ps[p[i]] = i;
            bool ok = true;
            for (int u = 0; u < n; u++) for (int v : g[u]) ok &= ps[u] < ps[v];
            if (ok) { best = p; break; }  // permutations come in lex order
        } while (next_permutation(all(p)));
        vi a = toposort(g), b = toposort(g, true), c = find_cycle(g);
        CHECK(a.empty() == best.empty() && c.empty() == !best.empty(), "acyclicity");
        CHECK(b == best, "lex smallest order");
        if (!a.empty()) {
            vi ps(n);
            for (int i = 0; i < n; i++) ps[a[i]] = i;
            for (int u = 0; u < n; u++) for (int v : g[u]) CHECK(ps[u] < ps[v], "valid order");
            vi lp = longest_path(g, a), memo(n, -1);
            auto rec = [&](auto &&self, int v) -> int {  // longest path ending at v
                if (memo[v] >= 0) return memo[v];
                int r = 0;
                for (int u = 0; u < n; u++) for (int x : g[u]) if (x == v) r = max(r, self(self, u) + 1);
                return memo[v] = r;
            };
            for (int v = 0; v < n; v++) CHECK(lp[v] == rec(rec, v), "longest path");
        } else {
            for (int i = 0; i < sz(c); i++) CHECK(count(all(g[c[i]]), c[(i + 1) % sz(c)]), "cycle edge");
            CHECK(sz(set<int>(all(c))) == sz(c), "simple cycle");
        }
    }
    int n = 200000;  // deep path: iterative DFS must not overflow
    vector<vi> g(n);
    for (int i = 0; i + 1 < n; i++) g[i].pb(i + 1);
    g[n - 1].pb(0);
    CHECK(sz(find_cycle(g)) == n && toposort(g).empty(), "big cycle");
    puts("OK");
}
