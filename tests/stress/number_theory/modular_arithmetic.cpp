// Stress test for src/number_theory/modular_arithmetic.cpp
// Oracles (different methods than the snippet):
//  - mulmod: Russian-peasant multiplication with modular additions
//  - powmod: naive repeated multiplication (small e) and e1+e2 split
//  - invmod: brute-force search (small m); a * inv == 1 (large m)
//  - Mint:   plain __int128 arithmetic, and x / y * y == x
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "tests/stress/common.h"

static ull addmod_slow(ull a, ull b, ull m) {  // (a + b) % m without overflow
    return a >= m - b ? a - (m - b) : a + b;
}
static ull mulmod_slow(ull a, ull b, ull m) {
    ull r = 0; a %= m;
    for (; b; b >>= 1, a = addmod_slow(a, a, m))
        if (b & 1) r = addmod_slow(r, a, m);
    return r;
}
static ull rnd_u64(ull lo, ull hi) {
    return std::uniform_int_distribution<ull>(lo, hi)(stress::rng);
}
static string S3(ull a, ull b, ull c) {
    return to_string(a) + " " + to_string(b) + " " + to_string(c);
}

int main() {
    stress::check_example(example_modular_arithmetic,
                          "src/number_theory/modular_arithmetic.cpp",
                          "example_modular_arithmetic");
    const ull BIG = (1ULL << 63) - 25;  // a large prime below 2^63
    // ---- edge cases ----
    CHECK(mulmod(0, 5, 7) == 0, "0*5 mod 7");
    CHECK(powmod(5, 0, 1) == 0, "x^0 mod 1 == 0");
    CHECK(powmod(0, 0, 7) == 1, "0^0 mod 7 == 1");
    CHECK(powmod(2, 62, BIG) == (1ULL << 62) % BIG, "2^62");
    CHECK(invmod(3, 11) == 4, "inv 3 mod 11");
    CHECK(invmod(2, 4) == -1, "no inverse");
    CHECK(invmod(-3, 11) == invmod(8, 11), "negative a");
    CHECK(invmod(5, 1) == 0, "mod 1");
    CHECK(mint(-1).v == MOD - 1, "Mint(-1)");
    CHECK((mint(2) / mint(2)).v == 1, "2/2");
    CHECK((-mint(0)).v == 0, "-0");

    // ---- mulmod vs Russian peasant (full 64-bit range) ----
    for (int it = 0; it < 200000; it++) {
        stress::iter = it;
        ull m = it % 3 ? rnd_u64(1, BIG) : rnd_u64(1, 1000);
        ull a = rnd_u64(0, m - 1), b = rnd_u64(0, m - 1);
        CHECK(mulmod(a, b, m) == mulmod_slow(a, b, m), S3(a, b, m));
    }
    // ---- powmod vs naive (small e) and split e = e1 + e2 ----
    for (int it = 0; it < 20000; it++) {
        stress::iter = it;
        ull m = rnd_u64(1, BIG), a = rnd_u64(0, ~0ULL);
        int e = (int)stress::rnd(0, 60);
        ull naive = 1 % m;
        for (int i = 0; i < e; i++) naive = mulmod_slow(naive, a % m, m);
        CHECK(powmod(a, e, m) == naive, S3(a, e, m));
        ull e1 = rnd_u64(0, 1ULL << 62), e2 = rnd_u64(0, 1ULL << 62);
        CHECK(powmod(a, e1 + e2, m) ==
                  mulmod_slow(powmod(a, e1, m), powmod(a, e2, m), m),
              S3(a, e1, e2) + " m=" + to_string(m));
    }
    // ---- invmod: brute force for small m ----
    for (ll m = 1; m <= 300; m++)
        for (ll a = -m; a <= 2 * m; a++) {
            ll want = -1, ar = ((a % m) + m) % m;
            for (ll x = 0; x < m; x++)
                if ((ar * x) % m == 1 % m) { want = x; break; }
            CHECK(invmod(a, m) == want, to_string(a) + " mod " + to_string(m));
        }
    // ---- invmod / invmod_prime on large moduli ----
    for (int it = 0; it < 20000; it++) {
        stress::iter = it;
        ll m = (ll)rnd_u64(2, BIG), a = (ll)rnd_u64(1, BIG);
        ll v = invmod(a, m);
        ll g = std::gcd(a, m);
        if (g != 1) CHECK(v == -1, S3(a, m, v));
        else CHECK(v >= 0 && v < m && mulmod(a % m, v, m) == 1, S3(a, m, v));
        ll p = (ll)BIG, b = (ll)rnd_u64(1, BIG - 1);
        CHECK(invmod_prime(b, p) == invmod(b, p), S3(b, p, 0));
    }
    // ---- Mint vs __int128 arithmetic ----
    for (int it = 0; it < 200000; it++) {
        stress::iter = it;
        ll x = stress::rnd(-4e18, 4e18), y = stress::rnd(-4e18, 4e18);
        __int128 M = MOD;
        auto norm = [&](__int128 z) { return (ll)(((z % M) + M) % M); };
        mint a = x, b = y;
        string in = to_string(x) + " " + to_string(y);
        CHECK((a + b).v == norm((__int128)norm(x) + norm(y)), in + " +");
        CHECK((a - b).v == norm((__int128)norm(x) - norm(y)), in + " -");
        CHECK((a * b).v == norm((__int128)norm(x) * norm(y)), in + " *");
        if (b != mint(0)) CHECK(a / b * b == a, in + " /");
        int e = (int)stress::rnd(0, 30);
        __int128 p = 1;
        for (int i = 0; i < e; i++) p = p * norm(x) % M;
        CHECK(a.pow(e).v == (ll)p, in + " pow " + to_string(e));
    }
    // ---- inv_table / batch_inv vs invmod, several primes ----
    for (ll p : {2LL, 3LL, 7LL, 1009LL, 998244353LL, (ll)MOD, 2147483647LL}) {
        int n = (int)min<ll>(p - 1, 200000);
        vl t = inv_table(n, p);
        for (int i = 1; i <= n; i++)
            CHECK(t[i] == invmod(i, p), to_string(i) + " mod " + to_string(p));
        for (int it = 0; it < 50; it++) {
            stress::iter = it;
            int len = (int)stress::rnd(0, 300);
            vl a(len);
            for (auto &x : a) {
                do x = stress::rnd(-4e18, 4e18); while (x % p == 0);
            }
            vl r = batch_inv(a, p);
            CHECK(sz(r) == len, "size");
            for (int i = 0; i < len; i++)
                CHECK(r[i] == invmod(a[i], p), to_string(a[i]) + " mod " + to_string(p));
        }
    }
    // ---- dmint (run-time modulus) vs __int128, random primes ----
    for (ll p : {2LL, 13LL, 1000003LL, 998244353LL, 2147483647LL}) {
        dmint::md = (int)p;
        for (int it = 0; it < 20000; it++) {
            stress::iter = it;
            ll x = stress::rnd(-4e18, 4e18), y = stress::rnd(-4e18, 4e18);
            auto nm = [&](__int128 z) { return (ll)(((z % p) + p) % p); };
            dmint a = x, b = y;
            string in = to_string(x) + " " + to_string(y) + " p=" + to_string(p);
            CHECK(a.v == nm(x), in + " ctor");
            CHECK((a + b).v == nm((__int128)nm(x) + nm(y)), in + " +");
            CHECK((a - b).v == nm((__int128)nm(x) - nm(y)), in + " -");
            CHECK((a * b).v == nm((__int128)nm(x) * nm(y)), in + " *");
            if (b != dmint(0)) CHECK(a / b * b == a, in + " /");
        }
    }
    // ---- Mint with another modulus ----
    using m7 = Mint<7>;
    for (int x = -50; x <= 50; x++) {
        CHECK(m7(x).v == ((x % 7) + 7) % 7, "Mint<7> " + to_string(x));
        if (x % 7) CHECK((m7(x) * m7(x).inv()).v == 1, "inv7 " + to_string(x));
        // negative exponents: x^-e == (x^-1)^e (the old pow looped forever)
        for (int e = -5; e <= 5 && x % 7; e++)
            CHECK(m7(x).pow(e) * m7(x).pow(-e) == m7(1), "pow neg " + to_string(e));
    }
    puts("OK");
}
