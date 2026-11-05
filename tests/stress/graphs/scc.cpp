// Stress test for src/graphs/scc.cpp
// Oracles: mutual reachability from a transitive closure (u ~ v iff u
// reaches v and v reaches u); every edge between components goes from a
// smaller to a larger id; dag() matches the distinct inter-component
// edges; a 2e5-node cycle plus tail checks the iterative version.
#include "tests/stress/template_lib.h"
#include "src/graphs/scc.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_scc, "src/graphs/scc.cpp", "example_scc");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 14), m = stress::rnd(0, 30);
        vector<vi> g(n);
        vector<vector<char>> r(n, vector<char>(n, 0));
        for (int i = 0; i < n; i++) r[i][i] = 1;
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            g[u].pb(v), r[u][v] = 1;
        }
        for (int k = 0; k < n; k++)
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++) r[i][j] |= r[i][k] && r[k][j];
        SCC s(g);
        set<int> ids(all(s.comp));
        CHECK(sz(ids) == s.ncomp && *ids.begin() == 0 && *ids.rbegin() == s.ncomp - 1, "ids");
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++)
                CHECK((s.comp[u] == s.comp[v]) == (r[u][v] && r[v][u]), "same component");
        set<pii> want;
        for (int u = 0; u < n; u++)
            for (int v : g[u])
                if (s.comp[u] != s.comp[v]) {
                    CHECK(s.comp[u] < s.comp[v], "topological ids");
                    want.insert({s.comp[u], s.comp[v]});
                }
        set<pii> got;
        auto d = s.dag(g);
        for (int c = 0; c < s.ncomp; c++) for (int x : d[c]) got.insert({c, x});
        CHECK(got == want && sz(d) == s.ncomp, "dag edges");
    }
    int n = 200000;
    vector<vi> g(n);
    for (int i = 0; i + 1 < n / 2; i++) g[i].pb(i + 1);
    g[n / 2 - 1].pb(0);
    for (int i = n / 2 - 1; i + 1 < n; i++) g[i].pb(i + 1);
    SCC s(g);
    CHECK(s.ncomp == n / 2 + 1 && s.comp[0] == s.comp[n / 2 - 1], "big");
    puts("OK");
}
