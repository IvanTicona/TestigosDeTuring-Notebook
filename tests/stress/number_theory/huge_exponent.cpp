// Stress test for src/number_theory/huge_exponent.cpp
// Oracles:
//  - powmod_str: exponent small enough to fit in __int128 / ll, compared
//    with plain fast exponentiation of the exact exponent; huge exponents:
//    a^(E1 E2) built two ways (string of E with E = E1 * 10^k + E2 split)
//  - tower: exact value computed with big-number-free brute force when the
//    tower is small (value fits in __int128), then reduced mod m; larger
//    towers: compare with powmod_str of the exponent written in decimal
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/multiplicative_functions.cpp"
#include "src/number_theory/huge_exponent.cpp"
#include "tests/stress/common.h"

static ll pw(ll a, unsigned __int128 e, ll m) {
    __int128 r = 1 % m, b = ((a % m) + m) % m;
    for (; e; e >>= 1, b = b * b % m) if (e & 1) r = r * b % m;
    return (ll)r;
}
static string to_str(unsigned __int128 x) {
    if (!x) return "0";
    string s;
    while (x) s += char('0' + x % 10), x /= 10;
    reverse(all(s));
    return s;
}
// exact tower value if it fits below LIM, else -1
static __int128 tower_exact(const vl &a, int i) {
    const __int128 LIM = (__int128)1 << 100;
    if (i == sz(a)) return 1;
    __int128 e = tower_exact(a, i + 1);
    if (e < 0) return a[i] == 1 ? 1 : -1;
    __int128 r = 1;
    for (__int128 k = 0; k < e; k++) {
        r *= a[i];
        if (r > LIM) return a[i] == 1 ? 1 : -1;
        if (a[i] == 1) break;
    }
    return r;
}

int main() {
    stress::check_example(example_huge_exponent, "src/number_theory/huge_exponent.cpp",
                          "example_huge_exponent");
    // ---- powmod_str vs exact exponent (fits in 128 bits) ----
    for (int it = 0; it < 100000; it++) {
        stress::iter = it;
        ll m = it % 4 == 0 ? stress::rnd(1, 50) : stress::rnd(1, 2000000000);
        if (it % 7 == 0) m = 1LL << stress::rnd(0, 40);
        ll a = stress::rnd(-1000000000, 1000000000);
        if (it % 5 == 0) a = m * stress::rnd(-3, 3);  // gcd(a, m) = m
        unsigned __int128 e = (unsigned __int128)stress::rnd(0, LLONG_MAX) * stress::rnd(0, 1000);
        if (it % 3 == 0) e = stress::rnd(0, 70);  // small exponents (E < phi)
        string in = to_string(a) + "^" + to_str(e) + " mod " + to_string(m);
        CHECK(powmod_str(a, to_str(e), m) == pw(a, e, m), in);
    }
    // ---- leading zeros and "0" ----
    CHECK(powmod_str(5, "0", 7) == 1 && powmod_str(5, "000", 7) == 1, "zero exponent");
    CHECK(powmod_str(0, "0", 7) == 1 && powmod_str(0, "5", 7) == 0, "zero base");
    // ---- huge exponents: a^(E) with E = X * 10^k, computed as (a^X)^(10^k) ----
    for (int it = 0; it < 2000; it++) {
        stress::iter = it;
        ll m = stress::rnd(1, 1000000000), a = stress::rnd(0, 1000000000);
        ll X = stress::rnd(0, 1000000000);
        int k = (int)stress::rnd(0, 300);
        string E = to_string(X) + string(k, '0');
        ll want = pw(a, X, m);  // (a^X)^(10^k) = repeated ^10, k times
        for (int i = 0; i < k; i++) want = pw(want, 10, m);
        CHECK(powmod_str(a, E, m) == want, to_string(a) + "^(" + to_string(X) + "e" + to_string(k) + ") mod " + to_string(m));
    }
    // ---- tower vs exact value ----
    for (int it = 0; it < 100000; it++) {
        stress::iter = it;
        int k = (int)stress::rnd(1, 4);
        vl a(k);
        for (auto &x : a) x = stress::rnd(1, it % 2 ? 5 : 30);
        ll m = it % 3 ? stress::rnd(1, 1000000000) : stress::rnd(1, 100);
        __int128 ex = tower_exact(a, 0);
        if (ex < 0) continue;
        string in = "tower k=" + to_string(k) + " m=" + to_string(m) + " a0=" + to_string(a[0]);
        CHECK(tower(a, m) == (ll)(ex % m), in);
    }
    // ---- tower vs powmod_str when the exponent tower is exact ----
    for (int it = 0; it < 20000; it++) {
        stress::iter = it;
        int k = (int)stress::rnd(2, 4);
        vl a(k);
        for (auto &x : a) x = stress::rnd(1, 9);
        a[0] = stress::rnd(1, 1000000000);
        __int128 e = tower_exact(a, 1);  // exponent of a[0]
        if (e < 0) continue;
        ll m = stress::rnd(1, 1000000000);
        CHECK(tower(a, m) == powmod_str(a[0], to_str(e), m), "tower vs str m=" + to_string(m));
    }
    puts("OK");
}
