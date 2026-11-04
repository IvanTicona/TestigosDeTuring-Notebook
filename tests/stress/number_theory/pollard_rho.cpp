// Stress test for src/number_theory/pollard_rho.cpp
// Oracles:
//  - is_prime vs an Eratosthenes sieve (n <= 2e6) and trial division
//    (random n <= 1e12); hard composites: Carmichael numbers, strong
//    pseudoprimes to the UMSS/UPB base sets, squares of large primes
//  - factor: product == n, every factor passes trial division / is_prime
//    cross-check, sorted; small n vs trial-division factorization
//  - speed: semiprimes of two ~1e9 primes and near-2^63 values
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/pollard_rho.cpp"
#include "tests/stress/common.h"

static bool isp_trial(ll n) {
    if (n < 2) return false;
    for (ll d = 2; d * d <= n; d++) if (n % d == 0) return false;
    return true;
}
static vl factor_trial(ll n) {
    vl r;
    for (ll d = 2; d * d <= n; d++) while (n % d == 0) r.pb(d), n /= d;
    if (n > 1) r.pb(n);
    return r;
}
static void check_factor(ll n) {
    vl f = factor(n);
    __int128 prod = 1;
    for (ll p : f) prod *= p;
    string in = "n=" + to_string(n);
    CHECK(prod == n, in + " product");
    CHECK(is_sorted(all(f)), in + " sorted");
    for (ll p : f) CHECK(is_prime(p), in + " factor " + to_string(p) + " prime");
    if (n <= 1000000000000LL)
        for (ll p : f) CHECK(isp_trial(p), in + " factor " + to_string(p) + " trial");
}

int main() {
    stress::check_example(example_pollard_rho, "src/number_theory/pollard_rho.cpp",
                          "example_pollard_rho");
    // ---- is_prime vs sieve ----
    const int N = 2000000;
    vector<char> sv(N + 1, 1);
    sv[0] = sv[1] = 0;
    for (int i = 2; (ll)i * i <= N; i++) if (sv[i]) for (int j = i * i; j <= N; j += i) sv[j] = 0;
    for (ll x = -5; x <= N; x++) CHECK(is_prime(x) == (x >= 0 && sv[x]), "x=" + to_string(x));
    // ---- is_prime vs trial division, random up to 1e12 ----
    for (int it = 0; it < 3000; it++) {
        stress::iter = it;
        ll n = stress::rnd(1, 1000000000000LL);
        CHECK(is_prime(n) == isp_trial(n), "n=" + to_string(n));
    }
    // ---- hard composites ----
    vl hard = {561, 1105, 1729, 2465, 2821, 6601, 8911, 41041, 825265,  // Carmichael
               2047, 3215031751LL, 341550071728321LL,                  // strong psp
               3825123056546413051LL,                                   // to bases 2..23
               4759123141LL,                                            // spsp(2,7,61)
               1000000007LL * 1000000007LL, 999999937LL * 999999929LL,
               3037000493LL * 3037000453LL};                            // near 2^63
    for (ll n : hard) {
        CHECK(!is_prime(n), "composite " + to_string(n));
        check_factor(n);
    }
    vl big_primes = {1000000007, 998244353, 2305843009213693951LL, 4611686018427387847LL,
                     9223372036854775783LL, 999999999999999989LL};
    for (ll p : big_primes) {
        CHECK(is_prime(p), "prime " + to_string(p));
        CHECK(factor(p) == vl{p}, "factor of prime " + to_string(p));
    }
    // ---- factor: small n vs trial division ----
    CHECK(factor(1).empty(), "factor(1)");
    for (ll n = 1; n <= 200000; n++) CHECK(factor(n) == factor_trial(n), "n=" + to_string(n));
    // ---- factor: random large n, products of random primes, prime powers ----
    for (int it = 0; it < 3000; it++) {
        stress::iter = it;
        ll n;
        int kind = it % 4;
        if (kind == 0) n = stress::rnd(2, LLONG_MAX);
        else if (kind == 1) {  // product of two primes near 1e9
            ll a = stress::rnd(5e8, 3e9), b = stress::rnd(5e8, 3e9);
            while (!is_prime(a)) a++;
            while (!is_prime(b)) b++;
            n = (ll)min<__int128>((__int128)a * b, LLONG_MAX);
        } else if (kind == 2) {  // several medium primes
            n = 1;
            while (true) {
                ll p = stress::rnd(2, 1000000);
                while (!is_prime(p)) p++;
                if ((__int128)n * p > LLONG_MAX) break;
                n *= p;
            }
        } else {  // prime power
            ll p = stress::rnd(2, 3000000);
            while (!is_prime(p)) p++;
            n = p;
            while ((__int128)n * p <= LLONG_MAX && stress::rnd(0, 3)) n *= p;
        }
        check_factor(n);
    }
    // ---- factor_pe groups factor ----
    for (int it = 0; it < 500; it++) {
        stress::iter = it;
        ll n = stress::rnd(1, LLONG_MAX);
        vl flat;
        for (auto [p, e] : factor_pe(n)) for (int i = 0; i < e; i++) flat.pb(p);
        CHECK(flat == factor(n), "pe n=" + to_string(n));
    }
    puts("OK");
}
