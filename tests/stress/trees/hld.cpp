// Stress test for src/trees/hld.cpp
// Oracles: naive path (parent walking) sums for vertex and edge modes, the
// set of positions covered by the ranges must be exactly the path (no
// overlaps), subtree ranges match descendants; the number of ranges per
// path stays <= 2 log2 n + 2; a 2e5-node path / caterpillar build.
#include "tests/stress/template_lib.h"
#include "src/trees/hld.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_hld, "src/trees/hld.cpp", "example_hld");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 70), root = stress::rnd(0, n - 1);
        vector<vi> g(n);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        int shape = stress::rnd(0, 2);
        bool forest = stress::rnd(0, 2) == 0;  // the old build crashed here
        for (int i = 1; i < n; i++) {
            if (forest && stress::rnd(0, 3) == 0) continue;
            int p = shape == 0 ? i - 1 : shape == 1 ? stress::rnd(max(0, i - 3), i - 1) : stress::rnd(0, i - 1);
            g[perm[i]].pb(perm[p]), g[perm[p]].pb(perm[i]);
        }
        HLD h(g, root);
        vi par(n, -1), dep(n, 0), q, comp(n, -1);
        for (int r0 = -1; r0 < n; r0++) {
            int r = r0 < 0 ? root : r0;
            if (comp[r] >= 0) continue;
            comp[r] = r, q.pb(r);
            for (int i = sz(q) - 1; i < sz(q); i++)
                for (int v : g[q[i]]) if (comp[v] < 0) comp[v] = r, par[v] = q[i], dep[v] = dep[q[i]] + 1, q.pb(v);
        }
        CHECK(sz(set<int>(all(h.pos))) == n, "positions are a permutation");
        int lg = 0;
        while ((1 << lg) < n) lg++;
        for (int t = 0; t < 60; t++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (comp[u] != comp[v]) continue;
            set<int> want;  // path vertices
            int a = u, b = v;
            while (dep[a] > dep[b]) want.insert(a), a = par[a];
            while (dep[b] > dep[a]) want.insert(b), b = par[b];
            while (a != b) want.insert(a), want.insert(b), a = par[a], b = par[b];
            int l = a;
            CHECK(h.lca(u, v) == l, "lca");
            for (bool edges : {false, true}) {
                set<int> got;
                int cnt = 0, ranges = 0;
                h.path(u, v, [&](int x, int y) {
                    ranges++;
                    for (int i = x; i <= y; i++) got.insert(i), cnt++;
                }, edges);
                set<int> w;
                for (int x : want) w.insert(h.pos[x]);
                if (!edges) w.insert(h.pos[l]);
                CHECK(got == w && cnt == sz(w), "path ranges edges=" + to_string(edges));
                CHECK(ranges <= 2 * lg + 2, "few ranges");
            }
        }
        for (int v = 0; v < n; v++) {
            auto [x, y] = h.subtree(v);
            int cnt = 0;
            for (int u = 0; u < n; u++) {
                bool d = false;
                for (int z = u; z >= 0; z = par[z]) d |= z == v;
                cnt += d;
                if (d) CHECK(x <= h.pos[u] && h.pos[u] <= y, "subtree contains");
            }
            CHECK(y - x + 1 == cnt, "subtree size");
        }
    }
    int n = 200000;
    vector<vi> g(n);
    for (int i = 1; i < n; i++) g[i - 1].pb(i), g[i].pb(i - 1);
    HLD h(g, 0);
    int ranges = 0;
    h.path(0, n - 1, [&](int, int) { ranges++; });
    CHECK(ranges == 1 && h.lca(n - 1, 7) == 7, "big path");
    puts("OK");
}
