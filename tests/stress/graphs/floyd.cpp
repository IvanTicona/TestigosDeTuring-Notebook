// Stress test for src/graphs/floyd.cpp
// Oracles: Bellman-Ford from every source (n rounds, then -inf spread) for all-pairs
// distances with negative edges; returned paths re-summed; DFS
// reachability for the closure; minimax by trying every threshold
// (union of edges <= t, connectivity) as an independent oracle.
#include "tests/stress/template_lib.h"
#include "src/graphs/floyd.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_floyd, "src/graphs/floyd.cpp", "example_floyd");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 9), m = stress::rnd(0, 22);
        bool neg = stress::rnd(0, 1);
        vector<vl> d(n, vl(n, INFL)), w0;
        for (int i = 0; i < n; i++) d[i][i] = 0;
        vector<array<ll, 3>> E;
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), w = stress::rnd(neg ? -3 : 0, 20);
            E.pb({u, v, w}), d[u][v] = min(d[u][v], w);
        }
        w0 = d;
        vector<vi> nxt;
        floyd(d, nxt);
        for (int s = 0; s < n; s++) {
            vl b(n, INFL);
            b[s] = 0;
            for (int r = 0; r < n; r++)
                for (auto [u, v, w] : E) if (b[u] < INFL && b[u] + w < b[v]) b[v] = b[u] + w;
            for (int r = 0; r < n; r++)
                for (auto [u, v, w] : E)
                    if (b[u] < INFL && (b[u] == -INFL || b[u] + w < b[v])) b[v] = -INFL;
            if (b[s] != -INFL) b[s] = min(b[s], 0LL);
            for (int t = 0; t < n; t++) {
                CHECK(d[s][t] == b[t], "dist " + to_string(s) + "->" + to_string(t));
                if (abs(d[s][t]) < INFL) {
                    vi p = fpath(nxt, s, t);
                    ll tot = 0;
                    for (int i = 0; i + 1 < sz(p); i++) tot += w0[p[i]][p[i + 1]];
                    CHECK(p[0] == s && p.back() == t && tot == d[s][t], "path sum");
                }
            }
        }
        vector<bitset<16>> r(n);
        for (int i = 0; i < n; i++) r[i][i] = 1;
        for (auto [u, v, w] : E) r[u][v] = 1;
        auto cl = closure<16>(r);
        for (int s = 0; s < n; s++) {
            vi vis(n, 0);
            vi st = {s};
            vis[s] = 1;
            while (!st.empty()) {
                int u = st.back();
                st.pop_back();
                for (auto [a, b, w] : E) if (a == u && !vis[b]) vis[b] = 1, st.pb(b);
            }
            for (int t = 0; t < n; t++) CHECK(cl[s][t] == vis[t], "closure");
        }
        // minimax on non-negative weights (directed)
        vector<vl> mw(n, vl(n, INFL));
        for (int i = 0; i < n; i++) mw[i][i] = 0;
        vector<array<ll, 3>> P;
        for (auto [u, v, w] : E) if (w >= 0) mw[u][v] = min(mw[u][v], w), P.pb({u, v, w});
        auto mm = minimax(mw);
        for (int s = 0; s < n; s++)
            for (int t = 0; t < n; t++) {
                ll want = s == t ? 0 : INFL;
                for (auto [u0, v0, th] : P) {  // smallest threshold connecting s -> t
                    vi vis(n, 0);
                    vi st = {s};
                    vis[s] = 1;
                    while (!st.empty()) {
                        int u = st.back();
                        st.pop_back();
                        for (auto [a, b, w] : P) if (a == u && w <= th && !vis[b]) vis[b] = 1, st.pb(b);
                    }
                    if (vis[t]) want = min(want, th);
                }
                CHECK(mm[s][t] == want, "minimax");
            }
    }
    puts("OK");
}
