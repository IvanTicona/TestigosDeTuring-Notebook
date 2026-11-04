// Stress test for src/number_theory/primitive_root.cpp
// Oracles: brute-force multiplicative order (repeated multiplication) for all
// small moduli; smallest generator found by brute force for small primes;
// known primitive roots of common NTT primes; generator property for large
// primes re-checked with an independent factorization and __int128 power.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/primitive_root.cpp"
#include "tests/stress/common.h"

static ll order_slow(ll a, ll m) {
    if (std::gcd(a, m) != 1) return -1;
    ll x = a % m, k = 1;
    while (x != 1 % m) x = x * a % m, k++;
    return k;
}
static bool is_prime_slow(ll n) {
    if (n < 2) return false;
    for (ll d = 2; d * d <= n; d++) if (n % d == 0) return false;
    return true;
}
static ll pw(ll a, ll e, ll m) {
    __int128 r = 1, b = a % m;
    for (; e; e >>= 1, b = b * b % m) if (e & 1) r = r * b % m;
    return (ll)r;
}

int main() {
    stress::check_example(example_primitive_root, "src/number_theory/primitive_root.cpp",
                          "example_primitive_root");
    // multiplicative order: every a for every m <= 400 (also negative a)
    for (ll m = 1; m <= 400; m++)
        for (ll a = -m; a < 2 * m; a++) {
            ll ar = ((a % m) + m) % m;
            CHECK(mult_order(a, m) == order_slow(ar, m), to_string(a) + " mod " + to_string(m));
        }
    // smallest primitive root of every prime < 5000
    for (ll p = 2; p < 5000; p++) {
        if (!is_prime_slow(p)) continue;
        ll want = 1;
        while (order_slow(want, p) != p - 1) want++;
        CHECK(primitive_root(p) == want, "p=" + to_string(p));
    }
    // known roots
    CHECK(primitive_root(998244353) == 3, "998244353");
    CHECK(primitive_root(1000000007) == 5, "1e9+7");
    CHECK(primitive_root(2147483647) == 7, "2^31-1");
    CHECK(primitive_root(167772161) == 3, "167772161");
    CHECK(primitive_root(469762049) == 3, "469762049");
    // large primes: generator property, and order of random a divides p - 1
    for (ll p : {998244353LL, 1000000007LL, 1000000009LL, 2147483647LL, 999999000001LL}) {
        CHECK(is_prime_slow(p), "prime " + to_string(p));
        ll g = primitive_root(p), n = p - 1;
        vector<ll> qs;
        for (ll d = 2; d * d <= n; d++) if (n % d == 0) { qs.pb(d); while (n % d == 0) n /= d; }
        if (n > 1) qs.pb(n);
        for (ll q : qs) CHECK(pw(g, (p - 1) / q, p) != 1, "gen " + to_string(p));
        for (int it = 0; it < 200; it++) {
            stress::iter = it;
            ll a = stress::rnd(1, p - 1), k = mult_order(a, p);
            CHECK((p - 1) % k == 0 && pw(a, k, p) == 1, to_string(a) + " order");
            for (ll q : qs)
                if (k % q == 0) CHECK(pw(a, k / q, p) != 1, to_string(a) + " minimal");
        }
    }
    puts("OK");
}
