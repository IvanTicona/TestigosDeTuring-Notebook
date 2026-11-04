// Stress test for src/number_theory/diophantine.cpp
// Oracles (brute force, different method):
//  - existence and smallest non-negative x: scan x = 0..|b| (or all x in a
//    window when b == 0) and test divisibility of c - a*x by b
//  - count_solutions: enumerate every x in the box, derive y, count
//  - lin_congruence: scan every x in [0, n)
//  - large values: identity a*x + b*y == c checked in __int128, 0 <= x < |b/g|
#include "tests/stress/template_lib.h"
#include "src/number_theory/extended_euclid.cpp"
#include "src/number_theory/diophantine.cpp"
#include "tests/stress/common.h"

static string S(ll a, ll b, ll c) { return to_string(a) + " " + to_string(b) + " " + to_string(c); }

static ll count_brute(ll a, ll b, ll c, ll x1, ll x2, ll y1, ll y2) {
    ll cnt = 0;
    for (ll x = x1; x <= x2; x++)
        for (ll y = y1; y <= y2; y++)
            if (a * x + b * y == c) cnt++;
    return cnt;
}

int main() {
    stress::check_example(example_diophantine, "src/number_theory/diophantine.cpp",
                          "example_diophantine");
    // ---- exhaustive small: existence + smallest non-negative x ----
    for (ll a = -25; a <= 25; a++)
        for (ll b = -25; b <= 25; b++)
            for (ll c = -40; c <= 40; c++) {
                ll x, y, g;
                bool ok = diophantine(a, b, c, x, y, g);
                bool want = false;
                ll want_x = -1;
                if (b != 0) {
                    for (ll t = 0; t <= llabs(b) && !want; t++)
                        if ((c - a * t) % b == 0) want = true, want_x = t;
                } else if (a != 0) {
                    want = c % a == 0;
                } else {
                    want = c == 0;
                }
                CHECK(ok == want, S(a, b, c) + " exists");
                if (!ok) continue;
                CHECK(a * x + b * y == c, S(a, b, c) + " identity");
                if (b != 0) CHECK(x == want_x, S(a, b, c) + " smallest x");
                if (a || b) CHECK(g == std::gcd(llabs(a), llabs(b)), S(a, b, c) + " g");
            }
    // ---- count_solutions vs enumeration over random small boxes ----
    for (int it = 0; it < 60000; it++) {
        stress::iter = it;
        ll a = stress::rnd(-12, 12), b = stress::rnd(-12, 12), c = stress::rnd(-60, 60);
        ll x1 = stress::rnd(-30, 30), x2 = stress::rnd(-30, 30);
        ll y1 = stress::rnd(-30, 30), y2 = stress::rnd(-30, 30);
        string in = S(a, b, c) + " box " + to_string(x1) + " " + to_string(x2) + " " +
                    to_string(y1) + " " + to_string(y2);
        CHECK(count_solutions(a, b, c, x1, x2, y1, y2) == count_brute(a, b, c, x1, x2, y1, y2), in);
    }
    // ---- lin_congruence vs scan ----
    for (ll n = 1; n <= 120; n++)
        for (ll a = -n; a <= n; a++)
            for (ll b = -n; b <= n; b += 3) {
                ll x, st;
                bool ok = lin_congruence(a, b, n, x, st);
                vector<ll> sols;
                for (ll t = 0; t < n; t++)
                    if ((((a * t - b) % n) + n) % n == 0) sols.pb(t);
                string in = S(a, b, n);
                CHECK(ok == !sols.empty(), in + " exists");
                if (!ok) continue;
                vector<ll> got;
                for (ll t = x; t < n; t += st) got.pb(t);
                CHECK(x >= 0 && x < st && got == sols, in + " all solutions");
            }
    // ---- large values: identity in __int128, x minimal ----
    const ll B = 1000000000000000000LL;
    for (int it = 0; it < 200000; it++) {
        stress::iter = it;
        ll k = it % 2 ? 1 : stress::rnd(1, 1000000);
        ll a = stress::rnd(-B / k, B / k) * k, b = stress::rnd(-B / k, B / k) * k;
        ll c = stress::rnd(-B, B);
        if (it % 3 == 0 && k > 1) c = stress::rnd(-B / k, B / k) * k;
        ll x, y, g;
        bool ok = diophantine(a, b, c, x, y, g);
        ll want_g = std::gcd(llabs(a), llabs(b));
        CHECK(ok == (want_g ? c % want_g == 0 : c == 0), S(a, b, c) + " exists");
        if (!ok) continue;
        CHECK((__int128)a * x + (__int128)b * y == c, S(a, b, c) + " identity");
        if (b) CHECK(x >= 0 && x < llabs(b / g), S(a, b, c) + " x range");
    }
    puts("OK");
}
