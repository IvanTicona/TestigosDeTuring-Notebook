// Stress test for src/misc/search.cpp
// Oracles: linear scans for first_true and the integer argmax (random
// strictly unimodal arrays with a plateau at the top); real search /
// ternary vs closed forms (roots, vertex of parabolas); compress vs
// std::map ranks; inversions vs the O(n^2) count.
#include "tests/stress/template_lib.h"
#include "src/misc/search.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_search, "src/misc/search.cpp", "example_search");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        ll lo = stress::rnd(-50, 50), hi = lo + stress::rnd(-1, 60), t = stress::rnd(lo - 2, hi + 2);
        ll want = hi + 1;
        for (ll x = lo; x <= hi; x++) if (x >= t) { want = x; break; }
        CHECK(first_true(lo, hi, [&](ll x) { return x >= t; }) == want, "first_true");
        // strictly unimodal array with a top plateau
        int n = stress::rnd(1, 40), peak = stress::rnd(0, n - 1), plat = stress::rnd(0, 3);
        vl a(n);
        a[peak] = 1000;
        for (int i = peak + 1; i < n; i++) a[i] = (i <= peak + plat) ? 1000 : a[i - 1] - stress::rnd(1, 5);
        for (int i = peak - 1; i >= 0; i--) a[i] = a[i + 1] - stress::rnd(1, 5);
        CHECK(argmax_int(0, n - 1, [&](ll i) { return a[i]; }) == peak, "argmax_int");
        ld c = stress::rnd(-1000, 1000) / 100.0L;
        ld xm = argmax_real(-20, 20, [&](ld x) { return -(x - c) * (x - c) + 3; });
        CHECK(fabsl(xm - c) < 1e-6, "argmax_real");
        ld root = stress::rnd(1, 10000) / 100.0L;
        ld r = real_search(0, 200, [&](ld x) { return x * x * x >= root * root * root; });
        CHECK(fabsl(r - root) < 1e-9, "real_search");
        vl v(stress::rnd(0, 30));
        for (ll &x : v) x = stress::rnd(-5, 5) * (ll)1e15;
        vl orig = v, vals = compress(v);
        map<ll, int> rk;
        for (ll x : orig) rk[x];
        int k = 0;
        for (auto &[x, id] : rk) id = k++;
        CHECK(sz(vals) == sz(rk), "distinct count");
        for (int i = 0; i < sz(v); i++) CHECK(v[i] == rk[orig[i]] && vals[v[i]] == orig[i], "ranks");
        ll inv = 0;
        for (int i = 0; i < sz(orig); i++) for (int j = i + 1; j < sz(orig); j++) inv += orig[i] > orig[j];
        CHECK(inversions(orig) == inv, "inversions");
    }
    puts("OK");
}
