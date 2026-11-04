// Stress test for src/number_theory/mobius_patterns.cpp
// Oracles: direct double loops with std::gcd (pair counts, ordered and
// unordered), square-free test by trial division, all-pairs gcd over the
// array, and the definitions sum n/gcd(i,n), sum gcd(i,n) by plain loops.
#include "tests/stress/template_lib.h"
#include "src/number_theory/multiplicative_functions.cpp"
#include "src/number_theory/floor_blocks.cpp"
#include "src/number_theory/mobius_patterns.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_mobius_patterns, "src/number_theory/mobius_patterns.cpp",
                          "example_mobius_patterns");
    const int N = 400;
    MobiusPrefix mp(N);
    // ---- pair counts vs double loop (exhaustive n, m <= 60; random <= 400) ----
    auto brute = [](ll n, ll m, ll k, bool unordered) {
        ll c = 0;
        for (ll i = 1; i <= n; i++)
            for (ll j = 1; j <= m; j++) {
                if (std::gcd(i, j) != k) continue;
                if (unordered && j < i && j <= n && i <= m) continue;  // (j, i) counted
                c++;
            }
        return c;
    };
    for (ll n = 0; n <= 60; n++)
        for (ll m = 0; m <= 60; m++) {
            string in = to_string(n) + " " + to_string(m);
            CHECK(mp.coprime_pairs(n, m) == brute(n, m, 1, false), in + " coprime");
            for (ll k = 1; k <= 7; k++) {
                CHECK(mp.gcd_pairs(n, m, k) == brute(n, m, k, false), in + " k=" + to_string(k));
                CHECK(mp.gcd_pairs_unordered(n, m, k) == brute(n, m, k, true), in + " unord k=" + to_string(k));
            }
        }
    for (int it = 0; it < 200; it++) {
        stress::iter = it;
        ll n = stress::rnd(0, N), m = stress::rnd(0, N), k = stress::rnd(1, 20);
        string in = to_string(n) + " " + to_string(m) + " k=" + to_string(k);
        CHECK(mp.gcd_pairs(n, m, k) == brute(n, m, k, false), in);
        CHECK(mp.gcd_pairs_unordered(n, m, k) == brute(n, m, k, true), in + " unord");
    }
    // ---- square-free count vs trial division ----
    ll c = 0;
    for (ll x = 1; x <= 30000; x++) {
        bool sf = true;
        for (ll p = 2; p * p <= x && sf; p++) sf = x % (p * p) != 0;
        c += sf;
        CHECK(squarefree_count(x) == c, "squarefree n=" + to_string(x));
    }
    // ---- gcd_pair_counts vs all pairs ----
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        int n = (int)stress::rnd(0, 60), V = (int)stress::rnd(1, 200);
        vi a(n);
        for (auto &x : a) x = (int)stress::rnd(1, V);
        vl want(V + 1, 0);
        for (int i = 0; i < n; i++) for (int j = i + 1; j < n; j++) want[std::gcd(a[i], a[j])]++;
        CHECK(gcd_pair_counts(a, V) == want, "array n=" + to_string(n) + " V=" + to_string(V));
    }
    // ---- sum n/gcd and Pillai vs definitions ----
    const int M = 2000;
    vl f = sum_n_over_gcd(M), g = pillai(M);
    for (ll n = 1; n <= M; n++) {
        ll a = 0, b = 0;
        for (ll i = 1; i <= n; i++) a += n / std::gcd(i, n), b += std::gcd(i, n);
        CHECK(f[n] == a && g[n] == b, "n=" + to_string(n));
    }
    puts("OK");
}
