// Stress test for src/dp/cht.cpp
// Oracle: max / min over every inserted line evaluated directly (__int128
// safe ranges), interleaving inserts and queries in random order; MonoCHT
// with slopes sorted non-increasing (duplicates included) and both query
// modes (random x and non-decreasing x).
#include "tests/stress/template_lib.h"
#include "src/dp/cht.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_cht, "src/dp/cht.cpp", "example_cht");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        ll K = stress::rnd(0, 1) ? 5 : 1000000, X = stress::rnd(0, 1) ? 10 : 1000000;
        ll M = K * X;
        LineContainer lc;
        vector<pll> ls;
        for (int op = 0; op < 80; op++) {
            if (ls.empty() || stress::rnd(0, 1)) {
                ll k = stress::rnd(-K, K), m = stress::rnd(-M, M);
                lc.add(k, m), ls.pb({k, m});
            } else {
                ll x = stress::rnd(-X, X), want = LLONG_MIN;
                for (auto [k, m] : ls) want = max(want, k * x + m);
                CHECK(lc.query(x) == want, "LineContainer x=" + to_string(x));
            }
        }
        // MonoCHT: sorted slopes, min
        int n = stress::rnd(1, 60);
        vector<pll> L(n);
        for (auto &[k, m] : L) k = stress::rnd(-K, K), m = stress::rnd(-M, M);
        sort(all(L), [](pll a, pll b) { return a.F > b.F; });
        MonoCHT h, h2;
        vl qs(40);
        for (ll &x : qs) x = stress::rnd(-X, X);
        sort(all(qs));
        int qi = 0;
        for (int i = 0; i < n; i++) {
            h.add(L[i].F, L[i].S), h2.add(L[i].F, L[i].S);
            ll x = stress::rnd(-X, X), want = LLONG_MAX;
            for (int j = 0; j <= i; j++) want = min(want, L[j].F * x + L[j].S);
            CHECK(h.query(x) == want, "MonoCHT query x=" + to_string(x));
            if (qi < sz(qs) && stress::rnd(0, 1)) {
                ll y = qs[qi++], w2 = LLONG_MAX;
                for (int j = 0; j <= i; j++) w2 = min(w2, L[j].F * y + L[j].S);
                CHECK(h2.query_inc(y) == w2, "MonoCHT query_inc x=" + to_string(y));
            }
        }
    }
    puts("OK");
}
