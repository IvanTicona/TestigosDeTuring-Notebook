// Stress test for src/number_theory/crt.cpp
// Oracles:
//  - crt, small moduli: scan x in [0, lcm) for the smallest x satisfying all
//    congruences (existence + exact value + lcm)
//  - crt, large moduli: every congruence re-checked with __int128, x in [0, l)
//  - garner: build a random 128-bit x, take residues, compare x % md
//    computed directly with __int128
#include "tests/stress/template_lib.h"
#include "src/number_theory/extended_euclid.cpp"
#include "src/number_theory/crt.cpp"
#include "tests/stress/common.h"

static string V(const vl &v) {
    string s = "{";
    for (ll x : v) s += to_string(x) + ",";
    return s + "}";
}

int main() {
    stress::check_example(example_crt, "src/number_theory/crt.cpp", "example_crt");
    // ---- small: exhaustive scan ----
    for (int it = 0; it < 30000; it++) {
        stress::iter = it;
        int k = (int)stress::rnd(1, 4);
        vl a(k), m(k);
        ll l = 1;
        for (int i = 0; i < k; i++) {
            m[i] = stress::rnd(1, 30);
            a[i] = stress::rnd(-100, 100);
            l = std::lcm(l, m[i]);
        }
        ll want = -1;
        for (ll x = 0; x < l && want < 0; x++) {
            bool ok = true;
            for (int i = 0; i < k; i++) ok &= ((x - a[i]) % m[i] + m[i]) % m[i] == 0;
            if (ok) want = x;
        }
        auto [x, ml] = crt(a, m);
        string in = V(a) + " " + V(m);
        if (want < 0) CHECK(x == -1 && ml == -1, in + " none");
        else CHECK(x == want && ml == l, in + " got " + to_string(x) + " " + to_string(ml));
    }
    // ---- large: lcm up to ~9e18 (the documented limit), verify congruences ----
    for (int it = 0; it < 100000; it++) {
        stress::iter = it;
        ll g = stress::rnd(1, 1000);  // common factor: lcm <= m1 * m2 / g <= 9e18
        ll m1 = stress::rnd(1, 3000000000LL / g) * g, m2 = stress::rnd(1, 3000000000LL / g) * g;
        ll x0 = stress::rnd(0, 4000000000000000000LL);
        ll a1 = x0 % m1, a2 = x0 % m2;  // consistent system
        auto [x, l] = crt({a1, a2}, {m1, m2});
        string in = to_string(a1) + " " + to_string(m1) + " " + to_string(a2) + " " + to_string(m2);
        CHECK(l == m1 / std::gcd(m1, m2) * m2, in + " lcm");
        CHECK(x >= 0 && x < l && x % m1 == a1 && x % m2 == a2, in + " congruences");
        auto bad = crt({a1, (a2 + 1) % m2}, {m1, m2});  // inconsistent iff g > 1
        if (std::gcd(m1, m2) > 1) CHECK(bad.F == -1, in + " should be inconsistent");
    }
    // ---- garner vs direct __int128 ----
    vl primes = {998244353, 1000000007, 1000000009, 754974721, 167772161};
    for (int it = 0; it < 50000; it++) {
        stress::iter = it;
        unsigned __int128 x = ((unsigned __int128)stress::rnd(0, LLONG_MAX) << 40) ^
                              (unsigned __int128)stress::rnd(0, LLONG_MAX);
        int k = (int)stress::rnd(1, 5);
        vl ps(primes.begin(), primes.begin() + k), res(k);
        unsigned __int128 prod = 1;
        for (int i = 0; i < k; i++) prod *= ps[i];
        x %= prod;  // x must be below the product to be recoverable
        for (int i = 0; i < k; i++) res[i] = (ll)(x % ps[i]);
        ll md = it % 2 ? stress::rnd(1, LLONG_MAX) : 1000000007;
        CHECK(garner(res, ps, md) == (ll)(x % md), V(res) + " md=" + to_string(md));
    }
    puts("OK");
}
