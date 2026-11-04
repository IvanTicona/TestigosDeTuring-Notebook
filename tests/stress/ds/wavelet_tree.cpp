// Stress test for src/ds/wavelet_tree.cpp
// Oracle: sort the subarray for kth, scan for count_le; values with
// duplicates, negatives and +-1e18; sizes 1..80.
#include "tests/stress/template_lib.h"
#include "src/ds/wavelet_tree.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_wavelet_tree, "src/ds/wavelet_tree.cpp", "example_wavelet_tree");
    for (stress::iter = 0; stress::iter < stress::scaled(500); stress::iter++) {
        int n = stress::rnd(1, 80);
        ll V = stress::rnd(0, 2) == 0 ? 3 : stress::rnd(0, 1) ? 1000 : (ll)1e18;
        vl a(n);
        for (ll &x : a) x = stress::rnd(-V, V);
        Wavelet w(a);
        for (int q = 0; q < 60; q++) {
            int l = stress::rnd(0, n - 1), r = stress::rnd(l, n - 1), k = stress::rnd(0, r - l);
            vl s(a.begin() + l, a.begin() + r + 1);
            sort(all(s));
            CHECK(w.kth(l, r, k) == s[k], "kth");
            ll x = stress::rnd(0, 1) ? s[stress::rnd(0, sz(s) - 1)] + stress::rnd(-1, 1) : stress::rnd(-V, V);
            CHECK(w.count_le(l, r, x) == upper_bound(all(s), x) - s.begin(), "count_le x=" + to_string(x));
        }
    }
    puts("OK");
}
