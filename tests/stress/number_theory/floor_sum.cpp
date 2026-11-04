// Stress test for src/number_theory/floor_sum.cpp
// Oracle: the direct O(n) sum computed in __int128 with a true floor
// division (negatives rounded toward -infinity). Results compared exactly
// when they fit in ll, and modulo 2^64 otherwise (the documented contract).
#include "tests/stress/template_lib.h"
#include "src/number_theory/floor_sum.cpp"
#include "tests/stress/common.h"

static i128 fdiv(i128 p, i128 q) { i128 r = p / q; return r - ((p % q != 0) && ((p < 0) != (q < 0))); }
static i128 brute_floor(ll n, ll m, ll a, ll b) {
    i128 s = 0;
    for (ll i = 0; i < n; i++) s += fdiv((i128)a * i + b, m);
    return s;
}
static i128 brute_mod(ll n, ll m, ll a, ll b) {
    i128 s = 0;
    for (ll i = 0; i < n; i++) { i128 x = ((i128)a * i + b) % m; s += x < 0 ? x + m : x; }
    return s;
}
static string S4(ll n, ll m, ll a, ll b) {
    return to_string(n) + " " + to_string(m) + " " + to_string(a) + " " + to_string(b);
}

int main() {
    stress::check_example(example_floor_sum, "src/number_theory/floor_sum.cpp", "example_floor_sum");
    // ---- small: exhaustive-ish, exact ----
    for (ll n = 0; n <= 20; n++)
        for (ll m = 1; m <= 12; m++)
            for (ll a = -15; a <= 15; a++)
                for (ll b = -15; b <= 15; b++) {
                    CHECK(floor_sum(n, m, a, b) == (ll)brute_floor(n, m, a, b), S4(n, m, a, b));
                    CHECK(mod_sum(n, m, a, b) == (ll)brute_mod(n, m, a, b), S4(n, m, a, b) + " mod");
                }
    // ---- random, up to the full ranges; compare mod 2^64 ----
    for (int it = 0; it < 2000; it++) {
        stress::iter = it;
        ll n = it % 2 ? stress::rnd(0, 1000) : stress::rnd(0, 300000);
        ll m = it % 3 ? stress::rnd(1, (1LL << 32) - 1) : stress::rnd(1, 1000);
        ll lim = it % 4 ? (ll)4e18 : 1000000000;
        ll a = stress::rnd(-lim, lim), b = stress::rnd(-lim, lim);
        if (n > 0 && (__int128)(llabs(a)) * n > (__int128)4e18) a /= n;  // keep a*i + b in ll range
        i128 want = brute_floor(n, m, a, b);
        CHECK((ull)floor_sum(n, m, a, b) == (ull)(unsigned __int128)want, S4(n, m, a, b));
        if (want >= LLONG_MIN && want <= LLONG_MAX) CHECK(floor_sum(n, m, a, b) == (ll)want, S4(n, m, a, b) + " exact");
    }
    puts("OK");
}
