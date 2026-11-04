// Stress test for src/combinatorics/lucas.cpp
// Oracles: exact Pascal triangle in unsigned __int128 (n <= 120) reduced
// mod p or mod m; Pascal mod m directly (n <= 1500, any m); for huge n,
// Lucas digit-by-digit vs binom_mod (two different methods), and the
// p^e version vs Kummer (vp_binom >= e  =>  0 mod p^e).
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/number_theory/extended_euclid.cpp"
#include "src/number_theory/crt.cpp"
#include "src/number_theory/legendre.cpp"
#include "src/combinatorics/binomial.cpp"
#include "src/combinatorics/lucas.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_lucas, "src/combinatorics/lucas.cpp", "example_lucas");
    const int E = 120;
    vector<vector<unsigned __int128>> ex(E + 1, vector<unsigned __int128>(E + 1, 0));
    for (int n = 0; n <= E; n++) {
        ex[n][0] = 1;
        for (int k = 1; k <= n; k++) ex[n][k] = ex[n - 1][k - 1] + ex[n - 1][k];
    }
    // ---- Lucas vs exact, small primes ----
    for (ll p : {2LL, 3LL, 5LL, 7LL, 11LL, 13LL, 97LL}) {
        Lucas lu(p);
        for (int n = 0; n <= E; n++)
            for (int k = -1; k <= n + 1; k++) {
                ll want = (k < 0 || k > n) ? 0 : (ll)(ex[n][k] % p);
                CHECK(lu.C(n, k) == want, "lucas C(" + to_string(n) + "," + to_string(k) + ") p=" + to_string(p));
            }
    }
    // ---- binom_mod vs exact, many moduli (prime powers, composites, 1) ----
    for (ll m = 1; m <= 200; m++)
        for (int n = 0; n <= 60; n++)
            for (int k = 0; k <= n; k++)
                CHECK(binom_mod(n, k, m) == (ll)(ex[n][k] % m), "C(" + to_string(n) + "," + to_string(k) + ") mod " + to_string(m));
    // ---- binom_mod vs Pascal mod m for larger n ----
    for (ll m : {720720LL, 142857LL, 1LL << 20, 1000000LL, 999999LL, 3LL * 3 * 3 * 3 * 3 * 3 * 3}) {
        const int N = 1500;
        vl row(1, 1 % m);
        for (int n = 1; n <= N; n++) {
            vl nr(n + 1);
            nr[0] = nr[n] = 1 % m;
            for (int k = 1; k < n; k++) nr[k] = (row[k - 1] + row[k]) % m;
            row = nr;
            if (n % 97 == 0 || n == N)
                for (int it = 0; it < 5; it++) {
                    int k = (int)stress::rnd(0, n);
                    CHECK(binom_mod(n, k, m) == row[k], "pascal C(" + to_string(n) + "," + to_string(k) + ") mod " + to_string(m));
                }
        }
    }
    // ---- huge n: Lucas vs binom_mod for prime m; Kummer zero rule ----
    for (ll p : {2LL, 3LL, 7LL, 13LL, 1009LL}) {
        Lucas lu(p);
        for (int it = 0; it < stress::scaled(300); it++) {
            stress::iter = it;
            ll n = stress::rnd(0, (ll)1e18), k = stress::rnd(0, n);
            string in = "C(" + to_string(n) + "," + to_string(k) + ") p=" + to_string(p);
            CHECK(lu.C(n, k) == binom_mod(n, k, p), in);
        }
    }
    for (int it = 0; it < stress::scaled(300); it++) {
        stress::iter = it;
        ll p = vl{2, 3, 5, 7}[stress::rnd(0, 3)], q = 1;
        int e = (int)stress::rnd(1, 6);
        for (int i = 0; i < e; i++) q *= p;
        ll n = stress::rnd(0, (ll)1e18), k = stress::rnd(0, n);
        ll r = binom_mod(n, k, q);
        string in = "C(" + to_string(n) + "," + to_string(k) + ") mod " + to_string(q);
        CHECK(r >= 0 && r < q, in + " range");
        ll v = vp_binom(n, k, p);
        if (v >= e) CHECK(r == 0, in + " kummer zero");
        else {  // exactly v factors of p: r / p^v must be coprime to p
            ll pv = 1;
            for (int i = 0; i < v; i++) pv *= p;
            CHECK(r % pv == 0 && (r / pv) % p != 0, in + " kummer exact power");
        }
    }
    puts("OK");
}
