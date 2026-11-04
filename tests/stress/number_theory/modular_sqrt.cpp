// Stress test for src/number_theory/modular_sqrt.cpp
// Oracles: for every small prime p (< 3000) and every a, brute-force square
// table (residue or not); for large primes, r*r == a checked with __int128
// and non-residues confirmed by Euler's criterion computed by hand.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/modular_sqrt.cpp"
#include "tests/stress/common.h"

static bool is_prime_slow(ll n) {
    if (n < 2) return false;
    for (ll d = 2; d * d <= n; d++) if (n % d == 0) return false;
    return true;
}
static ll pw(ll a, ll e, ll m) {  // independent __int128 power
    __int128 r = 1, b = a % m;
    for (; e; e >>= 1, b = b * b % m) if (e & 1) r = r * b % m;
    return (ll)r;
}

int main() {
    stress::check_example(example_modular_sqrt, "src/number_theory/modular_sqrt.cpp",
                          "example_modular_sqrt");
    // every residue of every small prime vs a brute-force table
    for (ll p = 2; p < 3000; p++) {
        if (!is_prime_slow(p)) continue;
        vector<char> is_sq(p, 0);
        for (ll x = 0; x < p; x++) is_sq[x * x % p] = 1;
        for (ll a = -p; a < 2 * p; a++) {
            ll ar = ((a % p) + p) % p, r = sqrtmod(a, p);
            string in = to_string(a) + " mod " + to_string(p);
            if (is_sq[ar]) CHECK(r >= 0 && r < p && r * r % p == ar, in);
            else CHECK(r == -1, in);
        }
    }
    // large primes, including p % 4 == 1 with a large power of two in p - 1
    vector<ll> big = {998244353, 1000000007, 1000000009, 2147483647, 167772161,
                      469762049, 2305843009213693951LL /* 2^61 - 1 */,
                      4611686018427387847LL /* < 2^62 */};
    for (ll p : big) {
        // trial division only where it is fast (the 2^61/2^62 primes were
        // verified once with Miller-Rabin while writing this test)
        if (p < 3000000000LL) CHECK(is_prime_slow(p), "not prime " + to_string(p));
        for (int it = 0; it < 3000; it++) {
            stress::iter = it;
            ll x = stress::rnd(0, p - 1), a = (ll)((__int128)x * x % p);
            ll r = sqrtmod(a, p);
            string in = to_string(a) + " mod " + to_string(p);
            CHECK(r >= 0 && (ll)((__int128)r * r % p) == a, in + " residue");
            ll b = stress::rnd(1, p - 1);
            ll rb = sqrtmod(b, p);
            if (pw(b, (p - 1) / 2, p) == 1)
                CHECK(rb >= 0 && (ll)((__int128)rb * rb % p) == b, to_string(b) + " res");
            else
                CHECK(rb == -1, to_string(b) + " non-residue mod " + to_string(p));
        }
    }
    puts("OK");
}
