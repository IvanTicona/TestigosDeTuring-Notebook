// Stress test for src/flows/push_relabel.cpp
// Oracles: brute-force min cut over all partitions (n <= 10, two-way edges,
// self-loops, parallel edges); left_of_cut must give a cut of that value;
// a big random graph compared against Dinic (independent algorithm).
#include "tests/stress/template_lib.h"
#include "src/flows/dinic.cpp"
#include "src/flows/push_relabel.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_push_relabel, "src/flows/push_relabel.cpp", "example_push_relabel");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(2, 10), m = stress::rnd(0, 25);
        int s = stress::rnd(0, n - 1), t = stress::rnd(0, n - 2);
        if (t >= s) t++;
        ll C = stress::rnd(0, 1) ? 5 : (ll)1e15;
        vector<array<ll, 4>> E;
        PushRelabel p(n);
        for (int i = 0; i < m; i++) {
            ll u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1), cp = stress::rnd(0, C);
            ll rc = stress::rnd(0, 3) == 0 ? stress::rnd(0, C) : 0;
            p.add_edge(u, v, cp, rc), E.pb({u, v, cp, rc});
        }
        auto cutcap = [&](int mask) {
            ll c = 0;
            for (auto [u, v, cp, rc] : E) {
                if (u == v) continue;
                if ((mask >> u & 1) && !(mask >> v & 1)) c += cp;
                if ((mask >> v & 1) && !(mask >> u & 1)) c += rc;
            }
            return c;
        };
        ll best = LLONG_MAX;
        for (int mask = 0; mask < (1 << n); mask++)
            if ((mask >> s & 1) && !(mask >> t & 1)) best = min(best, cutcap(mask));
        ll f = p.calc(s, t);
        CHECK(f == best, "maxflow == mincut");
        int side = 0;
        for (int v = 0; v < n; v++) side |= p.left_of_cut(v) << v;
        CHECK((side >> s & 1) && !(side >> t & 1) && cutcap(side) == f, "cut side");
    }
    for (int rep = 0; rep < 3; rep++) {
        int n = stress::SLOW ? 300 : 1500, m = n * 8;
        Dinic d(n);
        PushRelabel p(n);
        for (int i = 0; i < m; i++) {
            int u = stress::rnd(0, n - 1), v = stress::rnd(0, n - 1);
            ll c = stress::rnd(1, 1000000);
            d.add_edge(u, v, c), p.add_edge(u, v, c);
        }
        CHECK(d.maxflow(0, n - 1) == p.calc(0, n - 1), "big vs dinic");
    }
    puts("OK");
}
