// Stress test for src/combinatorics/binomial.cpp
// Oracles: Pascal recurrence computed independently in __int128 (exact) for
// n <= 120, reduced mod p; exact big products for binom_exact; brute-force
// definitions for P, multinomial (product of binomials) and stars and bars
// (counting solutions directly for small n, k); binom_big_n vs exact C
// reduced mod p for n < 120 and vs Comb for n < p, and Lucas splitting.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/combinatorics/binomial.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_binomial, "src/combinatorics/binomial.cpp", "example_binomial");
    // exact Pascal up to 120 in unsigned __int128 (C(120, 60) ~ 9.7e34 fits)
    const int E = 120;
    vector<vector<unsigned __int128>> ex(E + 1, vector<unsigned __int128>(E + 1, 0));
    for (int n = 0; n <= E; n++) {
        ex[n][0] = 1;
        for (int k = 1; k <= n; k++) ex[n][k] = ex[n - 1][k - 1] + ex[n - 1][k];
    }
    for (ll p : {2LL, 3LL, 7LL, 131LL, 998244353LL, (ll)MOD, 2147483647LL}) {
        int N = (int)min<ll>(p - 1, 3000);
        Comb cb(N, p);
        for (int n = -2; n <= min(N, E); n++)
            for (int k = -2; k <= n + 2; k++) {
                ll want = (n < 0 || k < 0 || k > n) ? 0 : (ll)(ex[n][k] % p);
                string in = "C(" + to_string(n) + "," + to_string(k) + ") mod " + to_string(p);
                CHECK(cb.C(n, k) == want, in);
                if (n >= 0 && k >= 0 && k <= n) {
                    unsigned __int128 perm = 1;
                    for (int i = 0; i < k; i++) perm = perm * (n - i) % p;
                    CHECK(cb.P(n, k) == (ll)perm, in + " P");
                }
            }
        for (int x = 1; x <= N; x++) CHECK(cb.inv(x) * x % p == 1 % p, "inv " + to_string(x));
        // multinomial = C(s, c1) * C(s - c1, c2) * ...
        for (int it = 0; it < stress::scaled(2000); it++) {
            stress::iter = it;
            int parts = (int)stress::rnd(0, 5);
            vl c(parts);
            ll s = 0;
            for (auto &x : c) x = stress::rnd(0, 20), s += x;
            if (s > N) continue;
            ll want = 1, rest = s;
            for (ll x : c) want = want * cb.C(rest, x) % p, rest -= x;
            CHECK(cb.multinomial(c) == want % p, "multinomial s=" + to_string(s));
        }
    }
    // stars and bars vs direct counting of x1 + ... + xk = n, xi >= 0
    Comb cb(1000);
    for (int k = 0; k <= 5; k++)
        for (int n = 0; n <= 12; n++) {
            function<ll(int, int)> ways = [&](int boxes, int left) -> ll {
                if (boxes == 0) return left == 0;
                ll s = 0;
                for (int x = 0; x <= left; x++) s += ways(boxes - 1, left - x);
                return s;
            };
            CHECK(cb.stars_bars(n, k) == ways(k, n) % MOD, "stars n=" + to_string(n) + " k=" + to_string(k));
        }
    // binom_exact vs exact Pascal
    for (int n = -1; n <= E; n++)
        for (int k = -1; k <= n + 1; k++) {
            i128 want = (n < 0 || k < 0 || k > n) ? 0 : (i128)ex[n][k];
            CHECK(binom_exact(n, k) == want, "exact C(" + to_string(n) + "," + to_string(k) + ")");
        }
    // binom_exact near the i128 limit (C(126, 63) overflowed the old version)
    {
        vector<vector<i128>> pa(131, vector<i128>(131, 0));
        for (int n = 0; n <= 130; n++) {
            pa[n][0] = 1;
            for (int k = 1; k <= n; k++) pa[n][k] = pa[n - 1][k - 1] + pa[n - 1][k];
            for (int k = 0; k <= n; k++) CHECK(binom_exact(n, k) == pa[n][k], "exact big C(" + to_string(n) + "," + to_string(k) + ")");
        }
    }
    // binom_big_n: small n vs exact; n < p vs Comb; huge n vs Lucas split
    for (ll p : {2LL, 3LL, 5LL, 13LL, 101LL}) {
        for (int n = 0; n <= E; n++)
            for (int k = 0; k <= n && k < p; k++)
                CHECK(binom_big_n(n, k, p) == (ll)(ex[n][k] % p), "big_n small p=" + to_string(p));
    }
    Comb big(200000);
    for (int it = 0; it < stress::scaled(20000); it++) {
        stress::iter = it;
        ll n = stress::rnd(0, 200000), k = stress::rnd(0, min<ll>(n, 2000));
        CHECK(binom_big_n(n, k, MOD) == big.C(n, k), "big_n vs Comb n=" + to_string(n));
        // huge n = q * p + r with small r: Lucas says C(n, k) == C(r, k) (k < p)
        ll q = stress::rnd(0, 999999999), r = stress::rnd(0, 200000), k2 = stress::rnd(0, 2000);
        ll N2 = q * MOD + r;
        CHECK(binom_big_n(N2, k2, MOD) == (r < k2 ? 0 : big.C(r, k2)), "lucas n=" + to_string(N2));
    }
    // pascal mod any m vs exact
    for (ll m : {1LL, 2LL, 6LL, 12LL, 1000000000000LL}) {
        auto c = pascal(E, m);
        for (int n = 0; n <= E; n++)
            for (int k = 0; k <= n; k++)
                CHECK(c[n][k] == (ll)(ex[n][k] % m), "pascal m=" + to_string(m));
    }
    puts("OK");
}
