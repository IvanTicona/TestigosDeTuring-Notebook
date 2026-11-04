// Stress test for src/dp/rerooting.cpp
// Oracles: BFS from every node of random trees (n <= 150) for the sum of
// distances, the eccentricity and the number of nodes at even distance;
// a 2e5-node path (deep tree) checked against closed forms.
#include "tests/stress/template_lib.h"
#include "src/dp/rerooting.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_rerooting, "src/dp/rerooting.cpp", "example_rerooting");
    auto add = [](pll a, pll b) { return pll{a.F + b.F, a.S + b.S}; };
    auto fin = [](pll a, int) { return pll{a.F + 1, a.S}; };
    auto lift = [](pll x, int, int) { return pll{x.F, x.S + x.F}; };
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 150);
        vector<vi> g(n);
        for (int i = 1; i < n; i++) {
            int p = stress::rnd(0, i - 1);
            g[i].pb(p), g[p].pb(i);
        }
        for (auto &l : g) shuffle(all(l), stress::rng);
        auto sum = reroot<pll>(g, {0, 0}, add, fin, lift);
        auto ecc = reroot<ll>(g, 0, [](ll a, ll b) { return max(a, b); },
                              [](ll a, int) { return a; }, [](ll x, int, int) { return x + 1; });
        // even: {#even-distance nodes, #odd} in the subtree; lift swaps parity
        auto even = reroot<pll>(g, {0, 0}, add, [](pll a, int) { return pll{a.F + 1, a.S}; },
                                [](pll x, int, int) { return pll{x.S, x.F}; });
        for (int s = 0; s < n; s++) {
            vi d(n, -1);
            d[s] = 0;
            queue<int> q;
            q.push(s);
            while (!q.empty()) {
                int u = q.front();
                q.pop();
                for (int v : g[u]) if (d[v] < 0) d[v] = d[u] + 1, q.push(v);
            }
            ll tot = 0, mx = 0, ev = 0;
            for (int x : d) tot += x, mx = max(mx, (ll)x), ev += x % 2 == 0;
            CHECK(sum[s] == pll(n, tot), "sum of distances s=" + to_string(s));
            CHECK(ecc[s] == mx, "eccentricity s=" + to_string(s));
            CHECK(even[s] == pll(ev, n - ev), "even distances s=" + to_string(s));
        }
    }
    int n = stress::SLOW ? 50000 : 200000;
    vector<vi> g(n);
    for (int i = 1; i < n; i++) g[i].pb(i - 1), g[i - 1].pb(i);
    auto sum = reroot<pll>(g, {0, 0}, add, fin, lift);
    for (int t = 0; t < 50; t++) {
        ll v = stress::rnd(0, n - 1), want = v * (v + 1) / 2 + (n - 1 - v) * (n - v) / 2;
        CHECK(sum[v].S == want, "path v=" + to_string(v));
    }
    puts("OK");
}
