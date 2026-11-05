// Stress test for src/flows/dinic.cpp
// Oracles: min cut by enumerating all 2^(n-2) source/sink partitions
// (n <= 10, two-way edges included); returned flows must respect
// capacities and conservation; the cut_side partition must have capacity
// == max flow; a second maxflow after adding edges continues correctly.
#include "tests/stress/template_lib.h"
#include "src/flows/dinic.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_dinic, "src/flows/dinic.cpp", "example_dinic");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(2, 10), m = stress::rnd(0, 25);
        int s = stress::rnd(0, n - 1), t = stress::rnd(0, n - 2);
        if (t >= s) t++;
        ll C = stress::rnd(0, 1) ? 5 : (ll)1e15;
        vector<array<ll, 4>> E;  // u, v, cap, rcap
        Dinic d(n);
        vi ids;
        auto cutcap = [&](int mask) {
            ll c = 0;  // mask bit = source side
            for (auto [u, v, cp, rc] : E) {
                if ((mask >> u & 1) && !(mask >> v & 1)) c += cp;
                if ((mask >> v & 1) && !(mask >> u & 1)) c += rc;
            }
            return c;
        };
        auto min_cut = [&]() {
            ll best = LLONG_MAX;
            for (int mask = 0; mask < (1 << n); mask++)
                if ((mask >> s & 1) && !(mask >> t & 1)) best = min(best, cutcap(mask));
            return best;
        };
        auto add = [&](int k) {
            for (int i = 0; i < k; i++) {
                ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), cp = stress::rnd(0, C);
                ll rc = stress::rnd(0, 3) == 0 ? stress::rnd(0, C) : 0;
                ids.pb(d.add_edge(u, v, cp, rc)), E.pb({u, v, cp, rc});
            }
        };
        add(m);
        ll f = d.maxflow(s, t);
        CHECK(f == min_cut(), "maxflow == mincut");
        int side = 0;
        for (int v = 0; v < n; v++) side |= d.cut_side(v) << v;
        CHECK((side >> s & 1) && !(side >> t & 1) && cutcap(side) == f, "cut side");
        vl bal(n, 0);
        for (int i = 0; i < sz(E); i++) {
            auto [u, v, cp, rc] = E[i];
            ll x = d.flow(ids[i]);
            CHECK(-rc <= x && x <= cp, "capacity");
            bal[u] -= x, bal[v] += x;
        }
        for (int v = 0; v < n; v++)
            CHECK(bal[v] == (v == t ? f : v == s ? -f : 0), "conservation");
        add(stress::rnd(0, 5));
        ll f2 = f + d.maxflow(s, t);
        CHECK(f2 == min_cut(), "incremental maxflow");
    }
    {  // 2e5-long augmenting path: the old recursive dfs blew the stack
        int n = 200000;
        Dinic d(n);
        ll mn = INFL;
        for (int i = 0; i + 1 < n; i++) {
            ll c = stress::rnd(5, (ll)1e12);
            mn = min(mn, c), d.add_edge(i, i + 1, c);
        }
        CHECK(d.maxflow(0, n - 1) == mn, "long chain");
    }
    puts("OK");
}
