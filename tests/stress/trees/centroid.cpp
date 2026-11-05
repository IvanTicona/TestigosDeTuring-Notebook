// Stress test for src/trees/centroid.cpp
// Oracles: BFS distances in the tree; each part of the decomposition (the
// nodes whose anc list contains c) is connected, has c as a centroid
// (every remaining piece <= half) and its depth is <= log2 n + 1; for all
// pairs, dist(u, v) = d(u, c) + d(v, c) with c the deepest common centroid
// ancestor; anc lists end with the node itself at distance 0.
#include "tests/stress/template_lib.h"
#include "src/trees/centroid.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_centroid, "src/trees/centroid.cpp", "example_centroid");
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 60), shape = stress::rnd(0, 2);
        vector<vi> g(n);
        vi perm(n);
        iota(all(perm), 0), shuffle(all(perm), stress::rng);
        for (int i = 1; i < n; i++) {
            int p = shape == 0 ? i - 1 : shape == 1 ? 0 : stress::rnd(0, i - 1);
            g[perm[i]].pb(perm[p]), g[perm[p]].pb(perm[i]);
        }
        vector<vi> D(n, vi(n, -1));
        for (int s = 0; s < n; s++) {
            vi q = {s};
            D[s][s] = 0;
            for (int i = 0; i < sz(q); i++)
                for (int v : g[q[i]]) if (D[s][v] < 0) D[s][v] = D[s][q[i]] + 1, q.pb(v);
        }
        Centroid cd(g);
        int lg = 0;
        while ((1 << lg) < n) lg++;
        int tops = 0;
        for (int v = 0; v < n; v++) {
            tops += cd.cpar[v] < 0;
            CHECK(sz(cd.anc[v]) <= lg + 1, "depth");
            CHECK(cd.anc[v].back() == pii(v, 0), "self last");
            for (auto [c, d] : cd.anc[v]) CHECK(D[v][c] == d, "distance");
            for (int i = 1; i < sz(cd.anc[v]); i++)
                CHECK(cd.cpar[cd.anc[v][i].F] == cd.anc[v][i - 1].F, "ancestor chain");
        }
        CHECK(tops == 1, "one top centroid");
        for (int c = 0; c < n; c++) {  // part of c: half-size pieces
            set<int> part;
            for (int v = 0; v < n; v++)
                for (auto [x, d] : cd.anc[v]) if (x == c) part.insert(v);
            for (int nb : g[c]) {
                if (!part.count(nb)) continue;
                int cnt = 0;  // piece through nb inside the part
                for (int v : part) cnt += v != c && D[c][v] == D[nb][v] + 1;
                CHECK(2 * cnt <= sz(part), "centroid property");
            }
        }
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++) {
                int k = 0;
                while (k < min(sz(cd.anc[u]), sz(cd.anc[v])) && cd.anc[u][k].F == cd.anc[v][k].F) k++;
                CHECK(k > 0 && cd.anc[u][k - 1].S + cd.anc[v][k - 1].S == D[u][v], "path through centroid");
            }
    }
    int n = 200000;
    vector<vi> g(n);
    for (int i = 1; i < n; i++) g[i - 1].pb(i), g[i].pb(i - 1);
    Centroid cd(g);
    CHECK(sz(cd.anc[0]) <= 19, "big path depth");
    puts("OK");
}
