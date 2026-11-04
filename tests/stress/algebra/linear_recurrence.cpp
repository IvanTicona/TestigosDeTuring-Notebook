// Stress test for src/algebra/linear_recurrence.cpp
// Oracles: random recurrences iterated directly (n <= 2000); BM output must
// reproduce the whole input sequence and be no longer than the generator;
// huge n checked against matrix exponentiation of the companion matrix
// (independent naive __int128 matrix product).
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/algebra/linear_recurrence.cpp"
#include "tests/stress/common.h"

typedef vector<vl> M;
M mul(const M &A, const M &B, ll p) {
    int n = sz(A);
    M C(n, vl(n, 0));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            i128 s = 0;
            for (int k = 0; k < n; k++) s += (i128)A[i][k] * B[k][j];
            C[i][j] = (ll)(s % p);
        }
    return C;
}
ll by_matrix(const vl &c, const vl &f, ll n, ll p) {
    int k = sz(c);
    if (n < k) return f[n];
    M T(k, vl(k, 0)), R(k, vl(k, 0));
    for (int j = 0; j < k; j++) T[0][j] = c[j];
    for (int i = 1; i < k; i++) T[i][i - 1] = 1;
    for (int i = 0; i < k; i++) R[i][i] = 1;
    for (ll e = n - k + 1; e; e >>= 1, T = mul(T, T, p))
        if (e & 1) R = mul(R, T, p);
    i128 s = 0;
    for (int j = 0; j < k; j++) s += (i128)R[0][j] * f[k - 1 - j];
    return (ll)(s % p);
}

int main() {
    stress::check_example(example_linear_recurrence, "src/algebra/linear_recurrence.cpp", "example_linear_recurrence");
    CHECK(berlekamp_massey({0, 0, 0}).empty() && kitamasa({}, {}, 5) == 0, "zero sequence");
    vl primes = {MOD, MOD2, 2147483647, 5, 2};
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        ll p = primes[stress::rnd(0, sz(primes) - 1)];
        int k = stress::rnd(1, 12);
        vl c(k), f(k);
        for (ll &x : c) x = stress::rnd(0, p - 1);
        for (ll &x : f) x = stress::rnd(0, p - 1);
        vl s = f;
        while (sz(s) < 2000) {
            ll v = 0;
            for (int j = 0; j < k; j++) v = (v + c[j] * s[sz(s) - 1 - j]) % p;
            s.pb(v);
        }
        for (int t = 0; t < 10; t++) {
            int n = stress::rnd(0, sz(s) - 1);
            CHECK(kitamasa(c, f, n, p) == s[n], "kitamasa k=" + to_string(k) + " n=" + to_string(n));
        }
        ll big = stress::rnd(0, (ll)1e18);
        CHECK(kitamasa(c, f, big, p) == by_matrix(c, f, big, p), "kitamasa big n=" + to_string(big));
        // BM: from a prefix of 2k + extra terms, reproduce all 2000
        vl pre(s.begin(), s.begin() + 2 * k + stress::rnd(0, 5));
        vl g = berlekamp_massey(pre, p);
        CHECK(sz(g) <= k, "BM order " + to_string(sz(g)) + " > " + to_string(k));
        for (int i = sz(g); i < sz(pre); i++) {
            ll v = 0;
            for (int j = 0; j < sz(g); j++) v = (v + g[j] * pre[i - 1 - j]) % p;
            CHECK(v == pre[i], "BM reproduces its input i=" + to_string(i));
        }
        for (int t = 0; t < 5; t++) {
            int n = stress::rnd(0, sz(s) - 1);
            CHECK(guess_nth(pre, n, p) == s[n], "guess_nth n=" + to_string(n) + " p=" + to_string(p));
        }
    }
    // BM on arbitrary sequences: output must reproduce the input
    for (stress::iter = 0; stress::iter < stress::scaled(300); stress::iter++) {
        vl s(stress::rnd(1, 40));
        for (ll &x : s) x = stress::rnd(0, 2) ? stress::rnd(0, MOD - 1) : 0;
        vl g = berlekamp_massey(s);
        CHECK(sz(g) <= sz(s), "BM order <= length");
        for (int i = sz(g); i < sz(s); i++) {
            ll v = 0;
            for (int j = 0; j < sz(g); j++) v = (v + g[j] * s[i - 1 - j]) % MOD;
            CHECK(v == s[i], "BM arbitrary i=" + to_string(i));
        }
    }
    puts("OK");
}
