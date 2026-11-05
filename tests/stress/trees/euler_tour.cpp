// Stress test for src/trees/euler_tour.cpp
// Oracles: naive parent walking for LCA and ancestry; subtree [tin, tout]
// must contain exactly the descendants (collected by BFS from v avoiding
// the parent); ord is a permutation consistent with tin; 2e5-node path.
#include "tests/stress/template_lib.h"
#include "src/trees/euler_tour.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_euler_tour, "src/trees/euler_tour.cpp", "example_euler_tour");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 60), root = stress::rnd(0, n - 1);
        vector<vi> g(n);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        for (int i = 1; i < n; i++) {
            int a = perm[i], b = perm[stress::rnd(0, i - 1)];
            g[a].pb(b), g[b].pb(a);
        }
        vi par(n, -1), dep(n, 0), q = {root};
        vector<char> seen(n, 0);
        seen[root] = 1;
        for (int i = 0; i < sz(q); i++)
            for (int v : g[q[i]]) if (!seen[v]) seen[v] = 1, par[v] = q[i], dep[v] = dep[q[i]] + 1, q.pb(v);
        EulerTour et(g, root);
        vi pos(n, -1);
        for (int i = 0; i < n; i++) pos[et.ord[i]] = i;
        for (int v = 0; v < n; v++) CHECK(pos[v] == et.tin[v], "ord / tin");
        auto anc = [&](int u, int v) {  // u ancestor of v
            for (int x = v; x >= 0; x = par[x]) if (x == u) return true;
            return false;
        };
        for (int u = 0; u < n; u++) {
            int cnt = 0;
            for (int v = 0; v < n; v++) {
                bool a = anc(u, v);
                cnt += a;
                CHECK(et.is_anc(u, v) == a, "is_anc");
                if (a) CHECK(et.tin[u] <= et.tin[v] && et.tin[v] <= et.tout[u], "subtree range");
            }
            CHECK(et.tout[u] - et.tin[u] + 1 == cnt, "subtree size");
        }
        for (int t = 0; t < 100; t++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), a = u, b = v;
            while (dep[a] > dep[b]) a = par[a];
            while (dep[b] > dep[a]) b = par[b];
            while (a != b) a = par[a], b = par[b];
            CHECK(et.lca(u, v) == a, "lca");
        }
    }
    int n = 200000;
    vector<vi> g(n);
    for (int i = 1; i < n; i++) g[i - 1].pb(i), g[i].pb(i - 1);
    EulerTour et(g, 0);
    CHECK(et.lca(n - 1, 5) == 5 && et.tout[0] == n - 1, "big path");
    puts("OK");
}
