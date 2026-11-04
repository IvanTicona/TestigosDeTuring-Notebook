// Stress test for src/number_theory/sieves.cpp
// Oracles: trial-division primality and factorization (different method),
// prime counts recomputed with the independent odd sieve vs the linear
// sieve, segment results vs trial division, and the constant tables
// recomputed (pi up to 1e8 with odd_sieve; largest primes by trial division
// up to 1e12; max divisor counts up to 1e6 by a divisor-count sieve).
#include "tests/stress/template_lib.h"
#include "src/number_theory/sieves.cpp"
#include "tests/stress/common.h"

static bool isp_trial(ll n) {
    if (n < 2) return false;
    for (ll d = 2; d * d <= n; d++) if (n % d == 0) return false;
    return true;
}
// deterministic Miller-Rabin for 64-bit (bases valid for all n < 3.3e24)
static ll pw(ll a, ll e, ll m) {
    __int128 r = 1, b = a % m;
    for (; e; e >>= 1, b = b * b % m) if (e & 1) r = r * b % m;
    return (ll)r;
}
static bool isp(ll n) {
    if (n < 2) return false;
    for (ll p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37})
        if (n % p == 0) return n == p;
    ll d = n - 1; int s = 0;
    while (d % 2 == 0) d /= 2, s++;
    for (ll a : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37}) {
        ll x = pw(a, d, n);
        if (x == 1 || x == n - 1) continue;
        bool comp = true;
        for (int r = 1; r < s && comp; r++) {
            x = (ll)((__int128)x * x % n);
            if (x == n - 1) comp = false;
        }
        if (comp) return false;
    }
    return true;
}

int main() {
    stress::check_example(example_sieves, "src/number_theory/sieves.cpp", "example_sieves");
    // ---- linear sieve vs trial division, n = 0..2e5 ----
    const int N = 200000;
    Sieve sv(N);
    {
        Sieve s0(0), s1(1), s2(2);
        CHECK(s0.primes.empty() && s1.primes.empty() && s2.primes == vi{2}, "tiny sieves");
    }
    vi want;
    for (int x = 0; x <= N; x++) {
        bool p = isp_trial(x);
        CHECK(p == isp(x), "oracles agree " + to_string(x));
        if (p) want.pb(x);
        CHECK(sv.is_prime(x) == p, "is_prime " + to_string(x));
        if (x >= 2) {
            int d = 2;
            while (x % d) d++;
            CHECK(sv.spf[x] == d, "spf " + to_string(x));
            ll prod = 1, last = 0;
            for (auto [q, e] : sv.factorize(x)) {
                CHECK(isp(q) && q > last && e >= 1, "factor " + to_string(x));
                last = q;
                for (int i = 0; i < e; i++) prod *= q;
            }
            CHECK(prod == x, "factor product " + to_string(x));
        }
    }
    CHECK(sv.primes == want, "primes list");
    // ---- odd sieve vs linear sieve on every x <= N, all small n ----
    for (int n = 0; n <= 300; n++) {
        auto c = odd_sieve(n);
        for (int x = 0; x <= n; x++) CHECK(is_prime_odd(c, x) == isp(x), to_string(x) + " n=" + to_string(n));
    }
    {
        auto c = odd_sieve(N);
        for (int x = 0; x <= N; x++) CHECK(is_prime_odd(c, x) == sv.is_prime(x), "odd " + to_string(x));
    }
    // ---- PI_POW10 recomputed with the odd sieve (1e8; 1e7 under sanitizers,
    //      which are ~5x slower: still well below the runner timeout) ----
#ifdef _GLIBCXX_DEBUG
    const int PI_LIM = 10000000, PI_K = 8;
#else
    const int PI_LIM = 100000000, PI_K = 9;
#endif
    {
        auto c = odd_sieve(PI_LIM);
        ll cnt = 1, p10 = 10;
        int k = 1;
        for (ll x = 3; x <= PI_LIM; x += 2) {
            cnt += is_prime_odd(c, x);
            if (x + 1 == p10) {  // 10^k is even: pi(10^k) == pi(10^k - 1)
                CHECK(cnt == PI_POW10[k], "pi(10^" + to_string(k) + ")");
                k++, p10 *= 10;
            }
        }
        CHECK(k == PI_K, "checked pi up to the limit");
    }
    // ---- PRIME_BELOW_POW10 by trial division up to 1e12 ----
    for (int k = 1; k <= 12; k++) {
        ll p10 = 1;
        for (int i = 0; i < k; i++) p10 *= 10;
        ll p = PRIME_BELOW_POW10[k];
        CHECK(p < p10 && isp(p), "prime below 10^" + to_string(k));
        for (ll x = p + 1; x < p10; x++) CHECK(!isp(x), "gap 10^" + to_string(k));
    }
    // ---- MAX_DIVISORS_POW10 up to 1e6 by a divisor-count sieve ----
    {
        const int M = 1000000;
        vi d(M + 1, 0);
        for (int i = 1; i <= M; i++) for (int j = i; j <= M; j += i) d[j]++;
        int best = 0, p10 = 10, k = 1;
        for (int x = 1; x <= M; x++) {
            best = max(best, d[x]);
            if (x == p10) {
                CHECK(best == MAX_DIVISORS_POW10[k], "max divisors 10^" + to_string(k));
                k++, p10 *= 10;
            }
        }
    }
    // ---- segmented sieve vs trial division (random windows, large R) ----
    for (int it = 0; it < 300; it++) {
        stress::iter = it;
        ll R = it % 3 == 0 ? stress::rnd(0, 5000) : stress::rnd(1, 1000000000000LL);
        ll L = R - stress::rnd(0, it % 3 == 0 ? 5000 : 3000);
        if (it % 7 == 0) R = stress::rnd(0, 5000), L = stress::rnd(-50, 5);  // window from 0
        vl got = primes_in_range(L, R), exp;
        for (ll x = max(L, 0LL); x <= R; x++) if (isp(x)) exp.pb(x);
        CHECK(got == exp, "range [" + to_string(L) + ", " + to_string(R) + "]");
    }
    CHECK(primes_in_range(10, 5).empty() && primes_in_range(-5, 1).empty(), "empty ranges");
    puts("OK");
}
