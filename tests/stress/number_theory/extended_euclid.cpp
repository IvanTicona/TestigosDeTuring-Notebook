// Stress test for src/number_theory/extended_euclid.cpp
// Oracles: Bezout identity checked in __int128, gcd compared with a
// subtraction-free recursive gcd on |a|, |b|, and the documented bounds
// |x| <= |b|/g, |y| <= |a|/g. Small values: brute-force gcd by trial.
#include "tests/stress/template_lib.h"
#include "src/number_theory/extended_euclid.cpp"
#include "tests/stress/common.h"

static ll gcd_rec(ll a, ll b) { return b ? gcd_rec(b, a % b) : a; }
static ll gcd_trial(ll a, ll b) {
    a = llabs(a), b = llabs(b);
    if (!a || !b) return a + b;
    for (ll d = min(a, b); d >= 1; d--)
        if (a % d == 0 && b % d == 0) return d;
    return 1;
}
static void verify(ll a, ll b, ll want_g) {
    ll x = 7, y = 7;
    ll g = extgcd(a, b, x, y);
    string in = to_string(a) + " " + to_string(b);
    CHECK(g == want_g, in + " g=" + to_string(g));
    CHECK((__int128)a * x + (__int128)b * y == g, in + " bezout");
    if (g) {
        CHECK(llabs(x) <= llabs(b) / g || (b == 0 && llabs(x) <= 1), in + " |x|");
        CHECK(llabs(y) <= llabs(a) / g || (a == 0 && llabs(y) <= 1), in + " |y|");
    } else {
        CHECK(x == 1 && y == 0, in + " documented (0, 0) result");
    }
}

int main() {
    stress::check_example(example_extended_euclid,
                          "src/number_theory/extended_euclid.cpp",
                          "example_extended_euclid");
    // edge cases
    verify(0, 0, 0);
    verify(0, 5, 5);
    verify(5, 0, 5);
    verify(0, -5, 5);
    verify(-5, 0, 5);
    verify(1, 1, 1);
    verify(-1, -1, 1);
    const ll B = 4000000000000000000LL;
    verify(B, B - 1, 1);
    verify(-B, B, B);
    // all small pairs vs trial division
    for (ll a = -60; a <= 60; a++)
        for (ll b = -60; b <= 60; b++) verify(a, b, gcd_trial(a, b));
    // random large values (also multiples of a common factor)
    for (int it = 0; it < 300000; it++) {
        stress::iter = it;
        ll a, b;
        if (it % 2) {
            a = stress::rnd(-B, B), b = stress::rnd(-B, B);
        } else {
            ll k = stress::rnd(1, 1000000);
            a = stress::rnd(-B / k, B / k) * k, b = stress::rnd(-B / k, B / k) * k;
        }
        verify(a, b, gcd_rec(llabs(a), llabs(b)));
    }
    puts("OK");
}
