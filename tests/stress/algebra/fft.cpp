// Stress test for src/algebra/fft.cpp
// Oracles: naive O(n*m) convolution (exact __int128 / mod m) on random
// vectors; for big sizes, a few random output indices computed naively.
#include "tests/stress/template_lib.h"
#include "src/algebra/fft.cpp"
#include "tests/stress/common.h"

vl naive(const vl &a, const vl &b, ll m) {  // m = 0: exact
    if (a.empty() || b.empty()) return {};
    vector<i128> c(sz(a) + sz(b) - 1, 0);
    for (int i = 0; i < sz(a); i++)
        for (int j = 0; j < sz(b); j++) {
            c[i + j] += (i128)a[i] * b[j];
            if (m) c[i + j] %= m;
        }
    return vl(all(c));
}
ll at(const vl &a, const vl &b, int k, ll m) {
    i128 s = 0;
    for (int i = max(0, k - sz(b) + 1); i <= min(k, sz(a) - 1); i++) s = (s + (i128)a[i] * b[k - i]) % m;
    return (ll)s;
}
vl rvec(int n, ll lo, ll hi) {
    vl v(n);
    for (ll &x : v) x = stress::rnd(lo, hi);
    return v;
}

int main() {
    stress::check_example(example_fft, "src/algebra/fft.cpp", "example_fft");
    CHECK(multiply({}, {1, 2}).empty() && conv_mod({1}, {}, 7).empty(), "empty");
    // exact multiply, small/medium, negative values allowed
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int n = stress::rnd(1, 300), m = stress::rnd(1, 300);
        ll V = stress::rnd(0, 1) ? 10 : 30000;
        vl a = rvec(n, -V, V), b = rvec(m, -V, V);
        CHECK(multiply(a, b) == naive(a, b, 0), "n=" + to_string(n) + " m=" + to_string(m));
        vd c = conv(vd(all(a)), vd(all(b)));
        vl e = naive(a, b, 0);
        for (int i = 0; i < sz(e); i++) CHECK(fabs(c[i] - e[i]) < 1e-3, "conv double");
    }
    // conv_mod, any modulus
    vl mods = {2, 3, 10, 1000, MOD, MOD2, 1000000000, 2147483647, 999999937};
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        ll md = mods[stress::rnd(0, sz(mods) - 1)];
        int n = stress::rnd(1, 400), m = stress::rnd(1, 400);
        vl a = rvec(n, 0, md - 1), b = rvec(m, 0, md - 1);
        if (stress::rnd(0, 3) == 0) a.assign(n, md - 1), b.assign(m, md - 1);  // worst case
        CHECK(conv_mod(a, b, md) == naive(a, b, md), "mod=" + to_string(md) + " n=" + to_string(n));
    }
    // big sizes: sampled indices
    for (int rep = 0; rep < 3; rep++) {
        int n = stress::SLOW ? 30000 : 200000;
        vl a = rvec(n, 0, MOD - 1), b = rvec(n + rep, 0, MOD - 1);
        vl c = conv_mod(a, b, MOD);
        vl sa = rvec(n, 0, 1000), sb = rvec(n, 0, 1000);
        vl d = multiply(sa, sb);
        CHECK(sz(c) == 2 * n + rep - 1 && sz(d) == 2 * n - 1, "big sizes");
        for (int t = 0; t < 30; t++) {
            int k = stress::rnd(0, sz(c) - 1);
            CHECK(c[k] == at(a, b, k, MOD), "big conv_mod k=" + to_string(k));
            int k2 = stress::rnd(0, sz(d) - 1);
            CHECK(d[k2] == at(sa, sb, k2, (ll)4e18), "big multiply k=" + to_string(k2));
        }
    }
    // correlate
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        int n = stress::rnd(1, 200), m = stress::rnd(1, n);
        vl a = rvec(n, -50, 50), b = rvec(m, -50, 50);
        vl r = correlate(a, b);
        CHECK(sz(r) == n - m + 1, "correlate size");
        for (int i = 0; i + m <= n; i++) {
            ll s = 0;
            for (int j = 0; j < m; j++) s += a[i + j] * b[j];
            CHECK(r[i] == s, "correlate i=" + to_string(i));
        }
    }
    puts("OK");
}
