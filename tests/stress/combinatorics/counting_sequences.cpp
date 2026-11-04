// Stress test for src/combinatorics/counting_sequences.cpp
// Oracles (brute force by definition, small n):
//  - derangements: enumerate all permutations (n <= 9)
//  - stirling2 / bell: enumerate all set partitions via restricted growth
//    strings (n <= 10); stirling1: count cycles of every permutation (n <= 9)
//  - partitions: coin-change DP over part sizes (different from pentagonal)
//  - larger n: exact __int128 recurrences compared mod several m
//  - stirling2_single: vs the stirling2 table
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/combinatorics/binomial.cpp"
#include "src/combinatorics/counting_sequences.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_counting_sequences, "src/combinatorics/counting_sequences.cpp",
                          "example_counting_sequences");
    const ll BIG = (ll)4e18;
    // ---- permutations: derangements and cycle counts ----
    {
        vl d = derangements(9, BIG);
        auto s = stirling1(9, BIG);
        for (int n = 0; n <= 9; n++) {
            vi p(n);
            iota(all(p), 0);
            ll der = 0;
            vl cyc(n + 1, 0);
            do {
                bool fixed = false;
                for (int i = 0; i < n; i++) fixed |= p[i] == i;
                der += !fixed;
                vector<bool> seen(n);
                int c = 0;
                for (int i = 0; i < n; i++)
                    if (!seen[i]) { c++; for (int j = i; !seen[j]; j = p[j]) seen[j] = true; }
                cyc[c]++;
            } while (next_permutation(all(p)));
            CHECK(d[n] == der, "derangements n=" + to_string(n));
            for (int k = 0; k <= n; k++) CHECK(s[n][k] == cyc[k], "s1(" + to_string(n) + "," + to_string(k) + ")");
        }
    }
    // ---- set partitions via restricted growth strings ----
    {
        auto S = stirling2(10, BIG);
        vl b = bell(10, BIG);
        for (int n = 0; n <= 10; n++) {
            vl byk(n + 1, 0);
            vi a(n, 0);
            function<void(int, int)> rec = [&](int i, int mx) {  // mx = #blocks used
                if (i == n) { byk[mx]++; return; }
                for (int v = 0; v <= mx; v++) { a[i] = v; rec(i + 1, max(mx, v + 1)); }
            };
            rec(0, 0);
            ll tot = 0;
            for (int k = 0; k <= n; k++) {
                CHECK(S[n][k] == byk[k], "S2(" + to_string(n) + "," + to_string(k) + ")");
                tot += byk[k];
            }
            CHECK(b[n] == tot, "bell n=" + to_string(n));
        }
    }
    // ---- partitions vs coin-change DP ----
    {
        const int N = 400;
        vector<unsigned __int128> cc(N + 1, 0);
        cc[0] = 1;
        for (int part = 1; part <= N; part++) for (int n = part; n <= N; n++) cc[n] += cc[n - part];
        for (ll m : {1LL, 2LL, 1000LL, (ll)MOD, BIG}) {
            vl p = partitions(N, m);
            for (int n = 0; n <= N; n++) CHECK(p[n] == (ll)(cc[n] % m), "p(" + to_string(n) + ") m=" + to_string(m));
        }
    }
    // ---- tables vs exact __int128 recurrences for larger n, several m ----
    {
        const int N = 40;
        vector<vector<unsigned __int128>> S(N + 1, vector<unsigned __int128>(N + 1, 0)), s1 = S;
        vector<unsigned __int128> D(N + 1, 0);
        S[0][0] = s1[0][0] = 1, D[0] = 1;
        for (int n = 1; n <= N; n++) {
            for (int k = 1; k <= n; k++) {
                S[n][k] = k * S[n - 1][k] + S[n - 1][k - 1];
                s1[n][k] = (n - 1) * s1[n - 1][k] + s1[n - 1][k - 1];
            }
            if (n >= 2) D[n] = (n - 1) * (D[n - 1] + D[n - 2]);
        }
        for (ll m : {1LL, 7LL, 1000000LL, (ll)MOD, BIG}) {
            auto St = stirling2(N, m), st = stirling1(N, m);
            vl d = derangements(N, m), b = bell(N, m);
            for (int n = 0; n <= N; n++) {
                unsigned __int128 bs = 0;
                for (int k = 0; k <= n; k++) {
                    CHECK(St[n][k] == (ll)(S[n][k] % m), "S2 m=" + to_string(m));
                    // s1(40, k) can exceed 2^128? no: 40! ~ 8e47 does; reduce only
                    // where the exact value is meaningful (n <= 33: 33! ~ 8.7e36)
                    if (n <= 33) CHECK(st[n][k] == (ll)(s1[n][k] % m), "s1 m=" + to_string(m));
                    bs += S[n][k];
                }
                CHECK(b[n] == (ll)(bs % m), "bell n=" + to_string(n) + " m=" + to_string(m));
                if (n <= 33) CHECK(d[n] == (ll)(D[n] % m), "D n=" + to_string(n) + " m=" + to_string(m));
            }
        }
    }
    // ---- stirling2_single vs table ----
    {
        Comb cb(400);
        auto S = stirling2(400, MOD);
        for (int it = 0; it < stress::scaled(5000); it++) {
            stress::iter = it;
            ll n = stress::rnd(0, 400), k = stress::rnd(0, 400);
            CHECK(stirling2_single(cb, n, k) == S[n][k], "S2 single " + to_string(n) + "," + to_string(k));
        }
    }
    puts("OK");
}
