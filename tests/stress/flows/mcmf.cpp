// Stress test for src/flows/mcmf.cpp
// Oracles: (1) unit-by-unit successive shortest paths with plain
// Bellman-Ford on the residual graph (no potentials, no Dijkstra) giving
// the min cost of every flow value k; (2) on DAGs with NEGATIVE costs,
// enumeration of every integer edge-flow vector (small capacities) for the
// max flow of min cost. Edge flows checked for capacity, conservation and
// total cost.
#include "tests/stress/template_lib.h"
#include "src/flows/mcmf.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_mcmf, "src/flows/mcmf.cpp", "example_mcmf");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(2, 7), m = stress::rnd(0, 14);
        int s = stress::rnd(0, n - 1), t = stress::rnd(0, n - 2);
        if (t >= s) t++;
        vector<array<ll, 4>> E;
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (u == v) continue;
            E.pb({u, v, stress::rnd(0, 3), stress::rnd(0, 10)});
        }
        // oracle: plain residual graph, Bellman-Ford shortest path, 1 unit at a time
        struct R { int to; ll cap, cost; };
        vector<R> re;
        vector<vi> rg(n);
        for (auto [u, v, cp, c] : E) {
            rg[u].pb(sz(re)), re.pb({(int)v, cp, c});
            rg[v].pb(sz(re)), re.pb({(int)u, 0, -c});
        }
        vl costk = {0};  // costk[k] = min cost of flow k
        while (true) {
            vl d(n, INFL);
            vi pe(n, -1);
            d[s] = 0;
            for (int r = 0; r < n; r++)
                for (int u = 0; u < n; u++)
                    if (d[u] < INFL)
                        for (int id : rg[u])
                            if (re[id].cap > 0 && d[u] + re[id].cost < d[re[id].to])
                                d[re[id].to] = d[u] + re[id].cost, pe[re[id].to] = id;
            if (d[t] == INFL) break;
            for (int v = t; v != s; v = re[pe[v] ^ 1].to) re[pe[v]].cap--, re[pe[v] ^ 1].cap++;
            costk.pb(costk.back() + d[t]);
        }
        ll F = sz(costk) - 1;
        ll k = stress::rnd(0, 1) ? INFL : stress::rnd(0, F + 1);
        MCMF mc(n);
        vi ids;
        for (auto [u, v, cp, c] : E) ids.pb(mc.add_edge(u, v, cp, c));
        auto [f, c] = mc.solve(s, t, k);
        ll wantf = min(k, F);
        CHECK(f == wantf && c == costk[wantf], "flow " + to_string(f) + " cost " + to_string(c));
        vl bal(n, 0);
        ll tot = 0;
        for (int i = 0; i < sz(E); i++) {
            ll x = mc.flow(ids[i]);
            CHECK(0 <= x && x <= E[i][2], "capacity");
            bal[E[i][0]] -= x, bal[E[i][1]] += x, tot += x * E[i][3];
        }
        CHECK(tot == c, "edge flows sum to the cost");
        for (int v = 0; v < n; v++) CHECK(bal[v] == (v == t ? f : v == s ? -f : 0), "conservation");
    }
    // negative costs (DAG, so no negative cycles)
    for (stress::iter = 0; stress::iter < stress::scaled(800); stress::iter++) {
        int n = stress::rnd(2, 7), m = stress::rnd(0, 14);
        vector<array<ll, 4>> E;
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            if (u >= v) continue;
            E.pb({u, v, stress::rnd(0, 3), stress::rnd(-10, 10)});
        }
        MCMF a(n);
        for (auto [u, v, cp, c] : E) a.add_edge(u, v, cp, c);
        // oracle: enumerate all integer edge-flow vectors (small caps)
        ll bestc = INFL, bestf = -1;
        int me = sz(E);
        vl x(me, 0);
        auto rec = [&](auto &&self, int i) -> void {
            if (i == me) {
                vl bal(n, 0);
                ll c = 0;
                for (int j = 0; j < me; j++) bal[E[j][0]] -= x[j], bal[E[j][1]] += x[j], c += x[j] * E[j][3];
                for (int v = 1; v < n - 1; v++) if (bal[v]) return;
                ll f = bal[n - 1];
                if (f > bestf || (f == bestf && c < bestc)) bestf = f, bestc = c;
                return;
            }
            for (x[i] = 0; x[i] <= E[i][2]; x[i]++) self(self, i + 1);
            x[i] = 0;
        };
        if (me <= 8) {
            rec(rec, 0);
            auto [f, c] = a.solve(0, n - 1);
            CHECK(f == bestf && c == bestc, "negative costs");
        }
    }
    for (ll w : {1LL, -1LL}) {  // long chain: the old O(V E) Bellman-Ford
        int n = 50000;          // took ~20 s here; now skipped / 2 rounds
        MCMF m(n);
        for (int i = 0; i + 1 < n; i++) m.add_edge(i, i + 1, 2, w);
        auto t0 = chrono::steady_clock::now();
        auto [f, c] = m.solve(0, n - 1);
        double sec = chrono::duration<double>(chrono::steady_clock::now() - t0).count();
        CHECK(f == 2 && c == 2 * (n - 1) * w, "long chain");
        CHECK(sec < 2.0 || stress::SLOW, "long chain too slow");
    }
    puts("OK");
}
