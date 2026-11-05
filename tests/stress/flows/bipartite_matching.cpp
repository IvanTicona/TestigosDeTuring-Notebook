// Stress test for src/flows/bipartite_matching.cpp
// Oracles: max matching by trying every subset of edges (m <= 14); the
// returned matching must be valid; the cover must touch every edge and
// have size == matching (König); path cover vs enumeration of all subsets
// of DAG edges forming vertex-disjoint paths (in/out degree <= 1).
#include "tests/stress/template_lib.h"
#include "src/flows/bipartite_matching.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_bipartite_matching, "src/flows/bipartite_matching.cpp", "example_bipartite_matching");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int nl = stress::rnd(1, 7), nr = stress::rnd(1, 7), m = stress::rnd(0, 14);
        vector<pii> E;
        for (int i = 0; i < m; i++) E.pb({stress::rnd(0, nl - 1), stress::rnd(0, nr - 1)});
        int best = 0;
        for (int mask = 0; mask < (1 << m); mask++) {
            int ul = 0, ur = 0, c = 0;
            bool ok = true;
            for (int i = 0; i < m && ok; i++)
                if (mask >> i & 1) {
                    ok &= !(ul >> E[i].F & 1) && !(ur >> E[i].S & 1);
                    ul |= 1 << E[i].F, ur |= 1 << E[i].S, c++;
                }
            if (ok) best = max(best, c);
        }
        HopcroftKarp hk(nl, nr);
        for (auto [u, v] : E) hk.add_edge(u, v);
        int k = hk.solve();
        CHECK(k == best, "matching size");
        int cnt = 0;
        for (int u = 0; u < nl; u++)
            if (hk.ml[u] >= 0) {
                cnt++;
                CHECK(hk.mr[hk.ml[u]] == u && count(all(E), pii(u, hk.ml[u])), "valid pair");
            }
        CHECK(cnt == k, "ml count");
        auto [L, R] = hk.cover();
        CHECK(sz(L) + sz(R) == k, "König size");
        for (auto [u, v] : E) CHECK(count(all(L), u) || count(all(R), v), "cover touches edge");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 7), m = stress::rnd(0, 12);
        vector<pii> E;
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (u < v) E.pb({u, v});
        }
        int best = n;
        for (int mask = 0; mask < (1 << sz(E)); mask++) {
            vi in(n, 0), out(n, 0);
            int c = 0;
            bool ok = true;
            for (int i = 0; i < sz(E); i++)
                if (mask >> i & 1) ok &= !out[E[i].F]++ && !in[E[i].S]++, c++;
            if (ok) best = min(best, n - c);
        }
        CHECK(path_cover(n, E) == best, "path cover");
    }
    puts("OK");
}
