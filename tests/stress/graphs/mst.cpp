// Stress test for src/graphs/mst.cpp
// Oracle: enumerate every subset of edges (m <= 12) that is a spanning
// forest with the maximum number of edges (n - #components) and take the
// cheapest; chosen ids must form such a forest with the reported cost;
// prim_dense on the matrix form of the same graph (connected cases).
#include "tests/stress/template_lib.h"
#include "src/ds/dsu.cpp"
#include "src/graphs/mst.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_mst, "src/graphs/mst.cpp", "example_mst");
    for (stress::iter = 0; stress::iter < stress::scaled(1000); stress::iter++) {
        int n = stress::rnd(1, 7), m = stress::rnd(0, 12);
        vector<array<ll, 3>> E(m);
        for (auto &e : E) e = {stress::rnd(-10, 30), stress::rnd(0, n - 1), stress::rnd(0, n - 1)};
        DSU all_(n);
        for (auto &e : E) all_.unite(e[1], e[2]);
        int need = n - all_.comps;
        ll best = INFL;
        for (int mask = 0; mask < (1 << m); mask++) {
            if (__builtin_popcount(mask) != need) continue;
            DSU d(n);
            bool ok = true;
            ll c = 0;
            for (int i = 0; i < m; i++)
                if (mask >> i & 1) ok &= d.unite(E[i][1], E[i][2]), c += E[i][0];
            if (ok) best = min(best, c);
        }
        auto [cost, ids] = kruskal(n, E);
        CHECK(cost == best, "kruskal cost");
        CHECK(sz(ids) == need, "forest size");
        DSU d(n);
        ll c = 0;
        for (int i : ids) {
            CHECK(d.unite(E[i][1], E[i][2]), "no cycle");
            c += E[i][0];
        }
        CHECK(c == cost, "ids cost");
        vector<vl> w(n, vl(n, INFL));
        for (int i = 0; i < n; i++) w[i][i] = 0;
        for (auto &e : E) if (e[1] != e[2]) w[e[1]][e[2]] = w[e[2]][e[1]] = min(w[e[1]][e[2]], e[0]);
        CHECK(prim_dense(w) == (all_.comps == 1 ? best : -1), "prim_dense");
    }
    puts("OK");
}
