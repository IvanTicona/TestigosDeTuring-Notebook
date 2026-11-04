/**
 * Usage: Sieve sv(n); sv.primes; sv.spf[x]; sv.is_prime(x); sv.factorize(x)
 *        auto c = odd_sieve(n); is_prime_odd(c, x)   // n up to ~1e9
 *        primes_in_range(L, R)                       // R up to ~1e14
 * Complexity: Sieve O(n) (4 bytes per number, n <= ~5e7), factorize
 *             O(log x); odd_sieve O(n log log n), n / 16 bytes;
 *             primes_in_range O((R-L) log log R + sqrt(R))
 * Source: UPB PrimesSieve + UMSS Linear Sieve (linear sieve), UPB
 *         SegmentedSieve, UPB Primes (table); odd_sieve and tables: own
 * Note: primes_in_range needs a base sieve up to sqrt(R) and R - L + 1
 *       bytes: R ~1e14 and R - L ~1e7 is fine (UPB's "R up to 1e18" is not).
 * Status: untested
 */
// ---- Linear sieve: smallest prime factor of every x <= n ----
struct Sieve {
    vi spf, primes;
    Sieve(int n) : spf(n + 1, 0) {
        for (int i = 2; i <= n; i++) {
            if (!spf[i]) spf[i] = i, primes.pb(i);
            for (int p : primes) {
                if (p > spf[i] || (ll)p * i > n) break;
                spf[p * i] = p;  // each composite marked once, by its spf
            }
        }
    }
    bool is_prime(int x) const { return x >= 2 && spf[x] == x; }
    vpii factorize(int x) const {  // {prime, exponent}, increasing primes
        vpii f;
        while (x > 1) {
            int p = spf[x], e = 0;
            while (x % p == 0) x /= p, e++;
            f.pb({p, e});
        }
        return f;
    }
};

// ---- Odd-only Eratosthenes: c[i] == true iff 2i+1 is composite ----
vector<bool> odd_sieve(int n) {
    vector<bool> c(n / 2 + 1, false);
    c[0] = true;  // 1 is not prime
    for (ll i = 3; i * i <= n; i += 2)
        if (!c[i / 2])
            for (ll j = i * i; j <= n; j += 2 * i) c[j / 2] = true;
    return c;
}
bool is_prime_odd(const vector<bool> &c, ll x) {
    return x == 2 || (x > 2 && (x & 1) && !c[x / 2]);
}

// ---- Segmented sieve: all primes in [L, R] ----
vl primes_in_range(ll L, ll R) {
    if (R < 2 || L > R) return {};
    L = max(L, 2LL);
    ll s = sqrtl((long double)R);
    while (s * s > R) s--;
    while ((s + 1) * (s + 1) <= R) s++;
    vector<char> base(s + 1, 1), comp(R - L + 1, 0);
    for (ll p = 2; p <= s; p++) {
        if (!base[p]) continue;
        for (ll q = p * p; q <= s; q += p) base[q] = 0;
        for (ll q = max(p * p, (L + p - 1) / p * p); q <= R; q += p)
            comp[q - L] = 1;
    }
    vl res;
    for (ll x = L; x <= R; x++) if (!comp[x - L]) res.pb(x);
    return res;
}

// ---- Useful numbers (index k = power of 10) ----
// pi(10^k): number of primes <= 10^k
const ll PI_POW10[13] = {0, 4, 25, 168, 1229, 9592, 78498, 664579,
    5761455, 50847534, 455052511, 4118054813LL, 37607912018LL};
// largest prime < 10^k
const ll PRIME_BELOW_POW10[19] = {0, 7, 97, 997, 9973, 99991, 999983,
    9999991, 99999989, 999999937, 9999999967LL, 99999999977LL,
    999999999989LL, 9999999999971LL, 99999999999973LL, 999999999999989LL,
    9999999999999937LL, 99999999999999997LL, 999999999999999989LL};
// max number of divisors of any n <= 10^k (e.g. n <= 1e9: 1344)
const int MAX_DIVISORS_POW10[19] = {1, 4, 12, 32, 64, 128, 240, 448, 768,
    1344, 2304, 4032, 6720, 10752, 17280, 26880, 41472, 64512, 103680};

// ---- Example: call from solve(); each // is the line it prints ----
void example_sieves() {
    Sieve sv(100);
    cout << sz(sv.primes) << ' ' << sv.spf[91] << '\n';  // 25 7
    string f;
    for (auto [p, e] : sv.factorize(90))
        f += to_string(p) + "^" + to_string(e) + " ";
    cout << f << '\n';                               // 2^1 3^2 5^1
    auto c = odd_sieve(1000000);
    int cnt = 1;  // the prime 2
    for (int x = 3; x <= 1000000; x += 2) cnt += is_prime_odd(c, x);
    cout << cnt << '\n';                             // 78498
    string r;
    for (ll p : primes_in_range(1e12, 1e12 + 70))
        r += to_string(p % 1000) + " ";
    cout << r << '\n';                               // 39 61 63
}
