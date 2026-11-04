// Stress test for src/algebra/lagrange.cpp
// Oracles: random polynomials evaluated directly (Horner) at 0..n and at
// huge x; recovering random coefficients from distinct points; power sums
// by direct summation (N <= 2e5) and by Faulhaber closed forms for k <= 3
// at huge N (__int128).
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/lagrange.cpp"
#include "tests/stress/common.h"

ll ev(const vl &c, ll x, ll p) {
    x %= p;
    ll r = 0;
    for (int i = sz(c) - 1; i >= 0; i--) r = (r * x + c[i]) % p;
    return r;
}

int main() {
    stress::check_example(example_lagrange, "src/algebra/lagrange.cpp", "example_lagrange");
    vl primes = {MOD, MOD2, 101, 1009, 2147483647};
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        ll p = primes[stress::rnd(0, sz(primes) - 1)];
        int n = stress::rnd(0, min(60LL, p - 2));
        vl c(n + 1);
        for (ll &x : c) x = stress::rnd(0, p - 1);
        vl y(n + 1);
        for (int i = 0; i <= n; i++) y[i] = ev(c, i, p);
        ll x = stress::rnd(0, 3) ? stress::rnd(0, (ll)1e18) : stress::rnd(0, n + 5);
        CHECK(lagrange_seq(y, x, p) == ev(c, x, p), "p=" + to_string(p) + " n=" + to_string(n) + " x=" + to_string(x));
        // interpolate: n + 1 distinct points recover the coefficients
        set<ll> used;
        vl xs, ys;
        while (sz(xs) < n + 1) {
            ll v = stress::rnd(0, p - 1);
            if (used.insert(v).second) xs.pb(v), ys.pb(ev(c, v, p));
        }
        CHECK(interpolate(xs, ys, p) == c, "interpolate p=" + to_string(p) + " n=" + to_string(n));
    }
    // power sums: direct
    for (int k = 0; k <= 12; k++) {
        ll s = 0;
        for (ll N = 0; N <= (stress::SLOW ? 20000 : 200000); N++) {
            if (N) s = (s + powmod(N, k, MOD)) % MOD;
            if (N < 300 || N % 997 == 0) CHECK(power_sum(N, k) == s, "power_sum k=" + to_string(k) + " N=" + to_string(N));
        }
    }
    // power sums: Faulhaber for huge N
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        ll N = stress::rnd(0, (ll)1e18), p = MOD;
        i128 n = N % p;  // formulas hold mod p since 6 is invertible
        ll i2 = invmod_prime(2, p), i6 = invmod_prime(6, p);
        ll s1 = (ll)(n * (n + 1) % p * i2 % p);
        ll s2 = (ll)(n * (n + 1) % p * ((2 * n + 1) % p) % p * i6 % p);
        ll s3 = (ll)((i128)s1 * s1 % p);
        CHECK(power_sum(N, 0) == N % p && power_sum(N, 1) == s1 && power_sum(N, 2) == s2 &&
                  power_sum(N, 3) == s3, "Faulhaber N=" + to_string(N));
    }
    puts("OK");
}
