// Stress test for src/trees/virtual_tree.cpp
// Oracles: the node set must equal the closure of the keys under pairwise
// LCA (computed naively over all pairs, repeated until stable); each edge
// parent must be the nearest proper ancestor of the child inside the set
// (naive parent walking); |ver| - 1 edges, ver[0] is the top.
#include "tests/stress/template_lib.h"
#include "src/trees/euler_tour.cpp"
#include "src/trees/virtual_tree.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_virtual_tree, "src/trees/virtual_tree.cpp", "example_virtual_tree");
    for (stress::iter = 0; stress::iter < stress::scaled(1000); stress::iter++) {
        int n = stress::rnd(1, 40), root = stress::rnd(0, n - 1);
        vector<vi> g(n);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        for (int i = 1; i < n; i++) {
            int p = stress::rnd(0, i - 1);
            g[perm[i]].pb(perm[p]), g[perm[p]].pb(perm[i]);
        }
        vi par(n, -1), dep(n, 0), q = {root};
        vector<char> seen(n, 0);
        seen[root] = 1;
        for (int i = 0; i < sz(q); i++)
            for (int v : g[q[i]]) if (!seen[v]) seen[v] = 1, par[v] = q[i], dep[v] = dep[q[i]] + 1, q.pb(v);
        auto lca = [&](int a, int b) {
            while (dep[a] > dep[b]) a = par[a];
            while (dep[b] > dep[a]) b = par[b];
            while (a != b) a = par[a], b = par[b];
            return a;
        };
        int k = stress::rnd(1, n);
        vi keys;
        for (int i = 0; i < k; i++) keys.pb(stress::rnd(0, n - 1));  // duplicates allowed
        set<int> cl(all(keys));
        for (bool changed = true; changed;) {
            changed = false;
            vi cur(all(cl));
            for (int a : cur) for (int b : cur) changed |= cl.insert(lca(a, b)).second;
        }
        EulerTour et(g, root);
        auto [ver, es] = virtual_tree(et, keys);
        CHECK(set<int>(all(ver)) == cl && sz(ver) == sz(cl), "node set");
        CHECK(sz(es) == sz(ver) - 1, "edge count");
        for (auto [p, c] : es) {
            int x = par[c];
            while (x >= 0 && !cl.count(x)) x = par[x];
            CHECK(x == p, "nearest ancestor in the set");
        }
        for (int v : ver) CHECK(lca(ver[0], v) == ver[0], "ver[0] is the top");
    }
    puts("OK");
}
