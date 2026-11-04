// Stress test for src/number_theory/discrete_log.cpp
// Oracles:
//  - small m: brute force x = 0, 1, 2, ... (the sequence a^x mod m is
//    eventually periodic with pre-period + period <= m), exact smallest x
//  - large m: b = a^t for a random t; result x must satisfy a^x == b and
//    x <= t (it is the smallest one)
//  - discrete_root: brute-force existence over all x in [0, p) for small p;
//    returned root verified with an independent __int128 power
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/primitive_root.cpp"
#include "src/number_theory/discrete_log.cpp"
#include "tests/stress/common.h"

static ll pw(ll a, ll e, ll m) {
    __int128 r = 1 % m, b = a % m;
    for (; e; e >>= 1, b = b * b % m) if (e & 1) r = r * b % m;
    return (ll)r;
}
static ll dlog_brute(ll a, ll b, ll m) {
    __int128 cur = 1 % m;
    for (ll x = 0; x <= 2 * m + 2; x++) {
        if (cur == b) return x;
        cur = cur * a % m;
    }
    return -1;
}
static bool isp(ll n) {
    if (n < 2) return false;
    for (ll d = 2; d * d <= n; d++) if (n % d == 0) return false;
    return true;
}

int main() {
    stress::check_example(example_discrete_log, "src/number_theory/discrete_log.cpp",
                          "example_discrete_log");
    // ---- every (a, b) for every m <= 100 ----
    for (ll m = 1; m <= 100; m++)
        for (ll a = 0; a < m; a++)
            for (ll b = 0; b < m; b++) {
                string in = to_string(a) + "^x=" + to_string(b) + " mod " + to_string(m);
                CHECK(discrete_log(a, b, m) == dlog_brute(a, b, m), in);
            }
    CHECK(discrete_log(-1, -1, 7) == dlog_brute(6, 6, 7), "negative inputs");
    // ---- large m: planted solutions (each call is O(sqrt m): keep it few) ----
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        ll m = it % 10 == 0 ? stress::rnd(2, 1000000000000LL) : stress::rnd(2, 100000);
        if (it % 5 == 0) m = 1LL << stress::rnd(1, 40);  // gcd(a, m) > 1 often
        ll a = stress::rnd(0, m - 1), t = stress::rnd(0, 2000000);
        ll b = pw(a, t, m);
        ll x = discrete_log(a, b, m);
        string in = to_string(a) + "^x=" + to_string(b) + " mod " + to_string(m) + " t=" + to_string(t);
        CHECK(x >= 0 && x <= t && pw(a, x, m) == b, in);
        if (m <= 1000000) CHECK(x == dlog_brute(a, b, m), in + " smallest");
    }
    // ---- discrete_root ----
    for (ll p = 2; p < 200; p++) {
        if (!isp(p)) continue;
        for (ll k = 1; k <= 12; k++)
            for (ll a = 0; a < p; a++) {
                bool exists = false;
                for (ll x = 0; x < p && !exists; x++) exists = pw(x, k, p) == a;
                ll r = discrete_root(k, a, p);
                string in = "x^" + to_string(k) + "=" + to_string(a) + " mod " + to_string(p);
                if (!exists) CHECK(r == -1, in + " none");
                else CHECK(r >= 0 && r < p && pw(r, k, p) == a, in);
            }
    }
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        ll p = it % 2 ? 1000000007 : 998244353, k = stress::rnd(1, 1000);
        ll x0 = stress::rnd(1, p - 1), a = pw(x0, k, p);
        ll r = discrete_root(k, a, p);
        CHECK(r >= 0 && pw(r, k, p) == a, "large root k=" + to_string(k));
    }
    puts("OK");
}
