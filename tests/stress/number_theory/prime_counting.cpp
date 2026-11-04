// Stress test for src/number_theory/prime_counting.cpp
// Oracles: Eratosthenes sieve prefix counts and prefix sums (n <= 3e6),
// checked at EVERY value n / i of Lucy and for many n of PrimeCounter;
// known pi(10^k) (verified in the sieves test up to 1e8, OEIS A006880)
// for large n; Lucy and PrimeCounter cross-checked on random large n.
#include "tests/stress/template_lib.h"
#include "src/number_theory/prime_counting.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_prime_counting, "src/number_theory/prime_counting.cpp",
                          "example_prime_counting");
    const int N = 3000000;
    vector<char> sv(N + 1, 1);
    sv[0] = sv[1] = 0;
    for (int i = 2; (ll)i * i <= N; i++) if (sv[i]) for (int j = i * i; j <= N; j += i) sv[j] = 0;
    vl cnt(N + 1, 0);
    vector<i128> sum(N + 1, 0);
    for (int i = 1; i <= N; i++) cnt[i] = cnt[i - 1] + sv[i], sum[i] = sum[i - 1] + (sv[i] ? i : 0);
    PrimeCounter pc;
    // ---- every n <= 3000: Lucy at all n / i, PrimeCounter at n ----
    for (ll n = 0; n <= 3000; n++) {
        Lucy L(n);
        for (ll i = 1; i <= n; i++) {
            ll x = n / i;
            CHECK(L.count(x) == cnt[x] && L.sum(x) == sum[x], "n=" + to_string(n) + " x=" + to_string(x));
        }
        CHECK(pc.count_primes(n) == cnt[n], "pc n=" + to_string(n));
    }
    // ---- random n <= 3e6 ----
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        ll n = stress::rnd(3000, N);
        Lucy L(n), Lc(n, false);
        for (ll i = 1; i <= n; i = n / (n / i) + 1) {
            ll x = n / i;
            CHECK(L.count(x) == cnt[x] && L.sum(x) == sum[x], "n=" + to_string(n) + " x=" + to_string(x));
            CHECK(Lc.count(x) == cnt[x], "count-only n=" + to_string(n));
        }
        CHECK(pc.count_primes(n) == cnt[n], "pc n=" + to_string(n));
    }
    // ---- known values ----
    const ll PI10[] = {0, 4, 25, 168, 1229, 9592, 78498, 664579, 5761455, 50847534,
                       455052511, 4118054813LL, 37607912018LL};
    ll p10 = 1;
#ifdef _GLIBCXX_DEBUG
    const int KMAX_LUCY = 9, KMAX_PC = 10;   // sanitizers are much slower
#else
    const int KMAX_LUCY = 10, KMAX_PC = 12;
#endif
    for (int k = 1; k <= 12; k++) {
        p10 *= 10;
        if (k <= KMAX_LUCY) CHECK(Lucy(p10, false).count(p10) == PI10[k], "Lucy 10^" + to_string(k));
        if (k <= KMAX_PC) CHECK(pc.count_primes(p10) == PI10[k], "pc 10^" + to_string(k));
    }
    // ---- cross-check on random large n ----
    for (int it = 0; it < 6; it++) {
        stress::iter = it;
        ll n = stress::rnd(1000000000LL, 3000000000LL);
        CHECK(Lucy(n, false).count(n) == pc.count_primes(n), "cross n=" + to_string(n));
    }
    puts("OK");
}
