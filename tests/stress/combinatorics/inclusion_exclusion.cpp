// Stress test for src/combinatorics/inclusion_exclusion.cpp
// Oracles: direct scan over [1, N] (divisibility / gcd), and for exactly_k:
// random set systems over a small universe where S_j and the exact counts
// are computed by enumerating every subset of sets and every element.
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/combinatorics/binomial.cpp"
#include "src/combinatorics/inclusion_exclusion.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_inclusion_exclusion, "src/combinatorics/inclusion_exclusion.cpp",
                          "example_inclusion_exclusion");
    // ---- count_not_divisible vs scan ----
    for (int it = 0; it < stress::scaled(3000); it++) {
        stress::iter = it;
        ll N = stress::rnd(0, 3000);
        int m = (int)stress::rnd(0, 8);
        vl d(m);
        for (auto &x : d) x = stress::rnd(1, it % 2 ? 30 : 3000);
        ll want = 0;
        for (ll x = 1; x <= N; x++) {
            bool ok = true;
            for (ll v : d) ok &= x % v != 0;
            want += ok;
        }
        CHECK(count_not_divisible(N, d) == want, "N=" + to_string(N) + " m=" + to_string(m));
    }
    // huge N with large divisors (lcm overflow guard): compare with a
    // smaller equivalent instance shifted by whole lcm periods is hard; use
    // N up to 1e18 with tiny d and the periodic formula instead
    for (int it = 0; it < stress::scaled(2000); it++) {
        stress::iter = it;
        int m = (int)stress::rnd(1, 6);
        vl d(m);
        for (auto &x : d) x = stress::rnd(1, 12);
        ll L = 1;
        for (ll v : d) L = std::lcm(L, v);  // <= 27720
        ll N = stress::rnd(0, (ll)1e18), per = 0, rem = 0;
        for (ll x = 1; x <= L; x++) {
            bool ok = true;
            for (ll v : d) ok &= x % v != 0;
            per += ok;
            if (x <= N % L) rem += ok;
        }
        CHECK(count_not_divisible(N, d) == (N / L) * per + rem, "periodic N=" + to_string(N));
        vl big = d;
        big.pb(stress::rnd((ll)1e17, (ll)4e18));  // lcm with it can overflow ll
        ll got = count_not_divisible(N, big);
        CHECK(got >= 0 && got <= count_not_divisible(N, d), "overflow guard N=" + to_string(N));
    }
    // ---- count_coprime vs gcd scan ----
    for (int it = 0; it < stress::scaled(2000); it++) {
        stress::iter = it;
        ll x = stress::rnd(0, 3000), n = stress::rnd(1, it % 2 ? 1000 : (ll)1e12);
        ll want = 0;
        for (ll i = 1; i <= x; i++) want += std::gcd(i, n) == 1;
        CHECK(count_coprime(x, n) == want, "x=" + to_string(x) + " n=" + to_string(n));
    }
    // ---- exactly_k / at_least_one vs explicit sets ----
    for (int it = 0; it < stress::scaled(2000); it++) {
        stress::iter = it;
        int U = (int)stress::rnd(0, 12), m = (int)stress::rnd(0, 7);
        vi sets(m);
        for (auto &s : sets) s = (int)stress::rnd(0, (1 << U) - 1);
        vl S(m + 1, 0), exact(m + 1, 0);
        for (int mask = 0; mask < (1 << m); mask++) {
            int inter = (1 << U) - 1;
            for (int i = 0; i < m; i++) if (mask >> i & 1) inter &= sets[i];
            S[__builtin_popcount(mask)] += __builtin_popcount(inter);
        }
        int any = 0;
        for (int e = 0; e < U; e++) {
            int c = 0;
            for (int i = 0; i < m; i++) c += sets[i] >> e & 1;
            exact[c]++, any += c > 0;
        }
        for (int k = 0; k <= m; k++) CHECK(exactly_k(S, k) == exact[k], "exactly k=" + to_string(k));
        CHECK(at_least_one(S) == any, "at least one");
    }
    puts("OK");
}
