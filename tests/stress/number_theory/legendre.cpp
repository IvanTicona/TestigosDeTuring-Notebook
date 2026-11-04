// Stress test for src/number_theory/legendre.cpp
// Oracles:
//  - legendre: sum over i = 1..n of the exponent of p in i (n <= 3000),
//    and the digit-sum identity (n - s_p(n)) / (p - 1) for large n
//  - trailing_zeros_fact: n! built exactly as a big number in base b
//    (vector of digits, multiplied step by step) for small n, b
//  - vp_binom: counting carries of k + (n - k) in base p (Kummer, a
//    different method), and exact C(n, k) factor count for small n
#include "tests/stress/template_lib.h"
#include "src/number_theory/legendre.cpp"
#include "tests/stress/common.h"

static int vp(ll x, ll p) { int c = 0; while (x % p == 0) x /= p, c++; return c; }
static bool isp(ll n) {
    if (n < 2) return false;
    for (ll d = 2; d * d <= n; d++) if (n % d == 0) return false;
    return true;
}
static ll zeros_big(int n, int b) {  // n! in base b, count trailing zeros
    vector<int> dig = {1};           // little-endian digits in base b
    for (int i = 2; i <= n; i++) {
        ll carry = 0;
        for (auto &d : dig) { ll v = (ll)d * i + carry; d = (int)(v % b); carry = v / b; }
        while (carry) dig.pb((int)(carry % b)), carry /= b;
    }
    ll z = 0;
    while (z < sz(dig) && dig[z] == 0) z++;
    return z;
}
static int carries(ll a, ll b, ll p) {
    int c = 0, carry = 0;
    while (a || b || carry) {
        ll s = a % p + b % p + carry;
        carry = s >= p;
        c += carry;
        a /= p, b /= p;
    }
    return c;
}

int main() {
    stress::check_example(example_legendre, "src/number_theory/legendre.cpp", "example_legendre");
    vl primes;
    for (ll p = 2; p < 200; p++) if (isp(p)) primes.pb(p);
    // ---- legendre vs sum of exponents ----
    for (ll p : primes) {
        ll acc = 0;
        for (ll n = 0; n <= 3000; n++) {
            if (n) acc += vp(n, p);
            CHECK(vp_fact(n, p) == acc, "n=" + to_string(n) + " p=" + to_string(p));
        }
    }
    // ---- legendre vs digit-sum identity, large n ----
    for (int it = 0; it < 100000; it++) {
        stress::iter = it;
        ll n = stress::rnd(0, LLONG_MAX), p = primes[stress::rnd(0, sz(primes) - 1)];
        if (it % 3 == 0) p = 1000000007;
        ll s = 0, t = n;
        while (t) s += t % p, t /= p;
        CHECK(vp_fact(n, p) == (n - s) / (p - 1), "n=" + to_string(n) + " p=" + to_string(p));
    }
    // ---- trailing zeros vs exact big-number factorial ----
    for (int b = 2; b <= 60; b++)
        for (int n = 0; n <= 120; n += (n < 30 ? 1 : 7))
            CHECK(trailing_zeros_fact(n, b) == zeros_big(n, b), "n=" + to_string(n) + " b=" + to_string(b));
    // ---- vp_binom vs carries (Kummer) and exact small binomials ----
    for (int it = 0; it < 100000; it++) {
        stress::iter = it;
        ll n = it % 2 ? stress::rnd(0, 60) : stress::rnd(0, (ll)1e18);
        ll k = stress::rnd(0, n), p = primes[stress::rnd(0, sz(primes) - 1)];
        string in = "C(" + to_string(n) + "," + to_string(k) + ") p=" + to_string(p);
        CHECK(vp_binom(n, k, p) == carries(k, n - k, p), in);
    }
    for (ll n = 0; n <= 60; n++) {
        unsigned __int128 c = 1;  // exact C(n, k) fits for n <= 60 in 128 bits
        for (ll k = 0; k <= n; k++) {
            if (k) c = c * (n - k + 1) / k;
            for (ll p : {2, 3, 5, 7, 11, 13}) {
                int e = 0;
                unsigned __int128 x = c;
                while (x % p == 0) x /= p, e++;
                CHECK(vp_binom(n, k, p) == e, "exact C(" + to_string(n) + "," + to_string(k) + ")");
            }
        }
    }
    puts("OK");
}
