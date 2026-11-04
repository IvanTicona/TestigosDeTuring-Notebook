// Stress test for src/algebra/fwht.cpp
// Oracles: naive O(N^2) XOR/AND/OR convolutions (exact and mod p), naive
// O(3^n) subset convolution by submask enumeration, transform round trips.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/fwht.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_fwht, "src/algebra/fwht.cpp", "example_fwht");
    vl ps = {0, MOD, MOD2, 7};
    for (stress::iter = 0; stress::iter < stress::scaled(600); stress::iter++) {
        ll p = ps[stress::rnd(0, sz(ps) - 1)];
        int na = stress::rnd(1, 130), nb = stress::rnd(1, 130), op = stress::rnd(0, 2);
        ll V = p ? p - 1 : 1000;
        vl a(na), b(nb);
        for (ll &x : a) x = stress::rnd(p ? 0 : -V, V);
        for (ll &x : b) x = stress::rnd(p ? 0 : -V, V);
        vl c = bit_conv(a, b, op, p);
        int N = sz(c);
        CHECK(N >= max(na, nb) && (N & (N - 1)) == 0, "size");
        vl want(N, 0);
        for (int i = 0; i < na; i++)
            for (int j = 0; j < nb; j++) {
                int k = op == 0 ? i ^ j : op == 1 ? i & j : i | j;
                want[k] = p ? (want[k] + a[i] * b[j]) % p : want[k] + a[i] * b[j];
            }
        CHECK(c == want, "op=" + to_string(op) + " p=" + to_string(p) + " na=" + to_string(na) + " nb=" + to_string(nb));
        vl r = a;
        r.resize(N);
        vl t = r;
        fwht(t, op, false, p), fwht(t, op, true, p);
        CHECK(t == r, "round trip op=" + to_string(op));
    }
    for (stress::iter = 0; stress::iter < stress::scaled(200); stress::iter++) {
        int n = stress::rnd(0, 10), N = 1 << n;
        ll p = stress::rnd(0, 1) ? MOD : 7;
        vl f(N), g(N);
        for (ll &x : f) x = stress::rnd(0, p - 1);
        for (ll &x : g) x = stress::rnd(0, p - 1);
        vl h = subset_conv(f, g, p), want(N, 0);
        for (int S = 0; S < N; S++)
            for (int A = S;; A = (A - 1) & S) {
                want[S] = (want[S] + f[A] * g[S ^ A]) % p;
                if (!A) break;
            }
        CHECK(h == want, "subset_conv n=" + to_string(n));
    }
    puts("OK");
}
