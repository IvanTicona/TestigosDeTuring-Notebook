// Stress test for src/graphs/bridges.cpp
// Oracles (n <= 9, multi-edges and self-loops): an edge is a bridge iff
// removing it increases the number of components; v is a cut vertex iff
// comps(G - v) > comps(G); 2-edge components = connectivity without the
// bridges; BCCs: every non-loop edge in exactly one BCC, each BCC with
// >= 2 edges stays connected after deleting any one of its vertices, and
// two BCCs share at most one vertex, which must be a cut vertex.
#include "tests/stress/template_lib.h"
#include "src/graphs/bridges.cpp"
#include "tests/stress/common.h"

int comps(int n, const vector<pii> &E, int skip_e, int skip_v) {
    vi p(n);
    iota(all(p), 0);
    function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
    for (int i = 0; i < sz(E); i++) {
        if (i == skip_e || E[i].F == skip_v || E[i].S == skip_v) continue;
        p[f(E[i].F)] = f(E[i].S);
    }
    int c = 0;
    for (int v = 0; v < n; v++) if (v != skip_v && f(v) == v) c++;
    return c;
}

int main() {
    stress::check_example(example_bridges, "src/graphs/bridges.cpp", "example_bridges");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 9), m = stress::rnd(0, 14);
        vector<pii> E;
        for (int i = 0; i < m; i++) E.pb({stress::rnd(0, n - 1), stress::rnd(0, n - 1)});
        LowLink L(n, E);
        int base = comps(n, E, -1, -1);
        for (int i = 0; i < m; i++)
            CHECK(L.bridge[i] == (comps(n, E, i, -1) > base), "bridge " + to_string(i));
        for (int v = 0; v < n; v++)
            CHECK(L.cut[v] == (comps(n, E, -1, v) > base), "cut " + to_string(v));
        // 2-edge components: u ~ v iff connected after removing all bridges
        vector<pii> nb;
        for (int i = 0; i < m; i++) if (!L.bridge[i]) nb.pb(E[i]);
        vi p(n);
        iota(all(p), 0);
        function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
        for (auto [a, b] : nb) p[f(a)] = f(b);
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++) CHECK((L.ecomp[u] == L.ecomp[v]) == (f(u) == f(v)), "ecomp");
        // BCCs partition the non-loop edges
        vi cnt(m, 0);
        for (auto &c : L.bcc) for (int e : c) cnt[e]++;
        for (int i = 0; i < m; i++) CHECK(cnt[i] == (E[i].F != E[i].S), "edge in exactly one bcc");
        // a BCC with >= 2 edges has no cut vertex inside it (as a subgraph)
        for (auto &c : L.bcc) {
            set<int> vs;
            vector<pii> sub;
            for (int e : c) vs.insert(E[e].F), vs.insert(E[e].S), sub.pb(E[e]);
            for (int x : vs) {
                if (sz(c) < 2) break;
                set<int> rest;
                for (int y : vs) if (y != x) rest.insert(y);
                vi q(n);
                iota(all(q), 0);
                function<int(int)> g2 = [&](int z) { return q[z] == z ? z : q[z] = g2(q[z]); };
                for (auto [a, b] : sub) if (a != x && b != x) q[g2(a)] = g2(b);
                set<int> roots;
                for (int y : rest) roots.insert(g2(y));
                CHECK(sz(roots) == 1, "bcc is biconnected");
            }
        }
        for (int a = 0; a < sz(L.bcc); a++)  // maximality: BCCs meet at cut vertices
            for (int b = a + 1; b < sz(L.bcc); b++) {
                set<int> va, common;
                for (int e : L.bcc[a]) va.insert(E[e].F), va.insert(E[e].S);
                for (int e : L.bcc[b])
                    for (int x : {E[e].F, E[e].S}) if (va.count(x)) common.insert(x);
                CHECK(sz(common) <= 1, "two bccs share <= 1 vertex");
                for (int x : common) CHECK(L.cut[x], "shared vertex is a cut vertex");
            }
    }
    puts("OK");
}
