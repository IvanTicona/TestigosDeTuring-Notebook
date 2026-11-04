// Stress test for src/ds/mo.cpp
// Oracle: direct recomputation per query (distinct count and sum of squared
// frequencies) with both orderings (Hilbert and zig-zag blocks); the
// window must never shrink below empty (add/rem counters stay >= 0).
#include "tests/stress/template_lib.h"
#include "src/ds/mo.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_mo, "src/ds/mo.cpp", "example_mo");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 300), V = stress::rnd(1, 20), q = stress::rnd(1, 200);
        vi a(n);
        for (int &x : a) x = stress::rnd(0, V - 1);
        vector<pii> qs(q);
        for (auto &[l, r] : qs) l = stress::rnd(0, n - 1), r = stress::rnd(l, n - 1);
        for (bool hil : {true, false}) {
            vl cnt(V, 0);
            ll distinct = 0, sq = 0;
            bool neg = false;
            auto add = [&](int i) { sq += 2 * cnt[a[i]] + 1, distinct += cnt[a[i]]++ == 0; };
            auto rem = [&](int i) {
                if (cnt[a[i]] == 0) neg = true;
                sq -= 2 * cnt[a[i]] - 1, distinct -= --cnt[a[i]] == 0;
            };
            vl d = mo(n, qs, add, rem, [&] { return distinct; }, hil);
            cnt.assign(V, 0);  // mo() leaves the last window in place
            distinct = sq = 0;
            vl s = mo(n, qs, add, rem, [&] { return sq; }, hil);
            CHECK(!neg, "window went negative");
            for (int i = 0; i < q; i++) {
                vl c(V, 0);
                ll dd = 0, ss = 0;
                for (int k = qs[i].F; k <= qs[i].S; k++) c[a[k]]++;
                for (ll x : c) dd += x > 0, ss += x * x;
                CHECK(d[i] == dd && s[i] == ss, "query " + to_string(i) + " hilbert=" + to_string(hil));
            }
        }
    }
    puts("OK");
}
