// Stress test for src/algebra/ntt.cpp
// Oracles: naive O(n*m) convolution mod P / mod m (__int128) on random
// vectors (4 NTT primes, many moduli); transform round trip; big sizes
// checked on random output indices.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/ntt.cpp"
#include "tests/stress/common.h"

vl naive(const vl &a, const vl &b, ll m) {
    if (a.empty() || b.empty()) return {};
    vector<i128> c(sz(a) + sz(b) - 1, 0);
    for (int i = 0; i < sz(a); i++)
        for (int j = 0; j < sz(b); j++) c[i + j] = (c[i + j] + (i128)a[i] * b[j]) % m;
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
template <ll P, ll G>
void check_prime() {
    for (stress::iter = 0; stress::iter < stress::scaled(150); stress::iter++) {
        int n = stress::rnd(1, 300), m = stress::rnd(1, 300);
        vl a = rvec(n, 0, P - 1), b = rvec(m, 0, P - 1);
        CHECK((ntt_mul<P, G>(a, b)) == naive(a, b, P), "P=" + to_string(P) + " n=" + to_string(n));
        int len = 1 << stress::rnd(0, 10);
        vl x = rvec(len, 0, P - 1), y = x;
        ntt<P, G>(y), ntt<P, G>(y, true);
        CHECK(x == y, "round trip len=" + to_string(len));
    }
}

int main() {
    stress::check_example(example_ntt, "src/algebra/ntt.cpp", "example_ntt");
    CHECK(ntt_mul({}, {1}).empty() && conv_any({1}, {}, 10).empty(), "empty");
    check_prime<998244353, 3>();
    check_prime<167772161, 3>();
    check_prime<469762049, 3>();
    check_prime<754974721, 11>();
    // conv_any: any modulus, worst-case values (all m - 1)
    vl mods = {1, 2, 6, 1000, MOD, MOD2, 1000000000, 2147483647, 999999937};
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        ll md = mods[stress::rnd(0, sz(mods) - 1)];
        int n = stress::rnd(1, 300), m = stress::rnd(1, 300);
        vl a = rvec(n, 0, md - 1), b = rvec(m, 0, md - 1);
        if (stress::rnd(0, 3) == 0) a.assign(n, md - 1), b.assign(m, md - 1);
        CHECK(conv_any(a, b, md) == naive(a, b, md), "conv_any m=" + to_string(md));
    }
    // big: n = 2^19 output size, worst-case values for conv_any
    int n = stress::SLOW ? 30000 : 250000;
    vl a = rvec(n, 0, MOD2 - 1), b = rvec(n, 0, MOD2 - 1);
    vl c = ntt_mul(a, b);
    vl A(n, 2147483646), B(n, 2147483646);
    vl d = conv_any(A, B, 2147483647);
    for (int t = 0; t < 40; t++) {
        int k = stress::rnd(0, sz(c) - 1);
        CHECK(c[k] == at(a, b, k, MOD2), "big ntt k=" + to_string(k));
        CHECK(d[k] == at(A, B, k, 2147483647), "big conv_any k=" + to_string(k));
    }
    puts("OK");
}
