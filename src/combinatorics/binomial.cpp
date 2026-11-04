/**
 * Usage: Comb cb(N, p);  // p prime > N, p < 2^31
 *        cb.C(n, k), cb.P(n, k), cb.multinomial({c1, c2, ...}),
 *        cb.stars_bars(n, k)  (n identical balls into k boxes, empty ok),
 *        cb.inv(x)  (1/x for 1 <= x <= N)
 *        binom_big_n(n, k, p) -> C(n, k) mod p, n up to 1e18, k < p, O(k)
 *        binom_exact(n, k)    -> exact C(n, k) as __int128 (< ~1.7e38)
 *        pascal(N, m)[n][k]   -> C(n, k) mod ANY m (not only prime)
 * Complexity: Comb O(N) build, O(1) per query; pascal O(N^2)
 * Source: UPB CombinatoricsMod (base) + UMSS Modular Combinatorics (no
 *         bounds checks, 2 powmods per query) + UMSS Combinatorics (exact,
 *         overflowed in the intermediate product) + UPB BinomialDP_Pascal +
 *         UTP counting (perm_rep = multinomial)
 * Note: C = 0 if k < 0 or k > n. binom_big_n uses Lucas with k < p:
 *       C(n, k) == C(n mod p, k) (mod p).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: untested
 */
struct Comb {
    ll p;
    vl f, fi;  // n! and 1/n! mod p
    Comb(int N, ll p_ = MOD) : p(p_), f(N + 1), fi(N + 1) {
        f[0] = 1;
        for (int i = 1; i <= N; i++) f[i] = f[i - 1] * i % p;
        fi[N] = invmod_prime(f[N], p);
        for (int i = N; i > 0; i--) fi[i - 1] = fi[i] * i % p;
    }
    ll C(ll n, ll k) const {
        if (k < 0 || k > n || n < 0) return 0;
        return f[n] * fi[k] % p * fi[n - k] % p;
    }
    ll P(ll n, ll k) const {  // n! / (n-k)!
        if (k < 0 || k > n || n < 0) return 0;
        return f[n] * fi[n - k] % p;
    }
    ll multinomial(const vl &cnt) const {  // (sum c)! / prod(c!)
        ll s = 0, r = 1;
        for (ll c : cnt) s += c, r = r * fi[c] % p;
        return r * f[s] % p;
    }
    ll stars_bars(ll n, ll k) const {  // = C(n + k - 1, k - 1)
        if (k == 0) return n == 0;
        return C(n + k - 1, k - 1);
    }
    ll inv(ll x) const { return fi[x] * f[x - 1] % p; }
};

ll binom_big_n(ll n, ll k, ll p) {  // p prime, 0 <= k < p
    if (k < 0 || k > n) return 0;
    ll m = n % p;
    if (m < k) return 0;
    ll num = 1, den = 1;
    for (ll i = 0; i < k; i++) {
        num = (ll)mulmod(num, m - i, p);
        den = (ll)mulmod(den, i + 1, p);
    }
    return (ll)mulmod(num, invmod_prime(den, p), p);
}

i128 binom_exact(ll n, ll k) {
    if (k < 0 || k > n) return 0;
    k = min(k, n - k);
    i128 r = 1;
    for (ll i = 1; i <= k; i++) r = r * (n - k + i) / i;  // r = C(n-k+i, i)
    return r;
}

vector<vl> pascal(int N, ll m) {
    vector<vl> c(N + 1, vl(N + 1, 0));
    for (int n = 0; n <= N; n++) {
        c[n][0] = 1 % m;
        for (int k = 1; k <= n; k++)
            c[n][k] = (c[n - 1][k - 1] + c[n - 1][k]) % m;
    }
    return c;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_binomial() {
    Comb cb(200000);
    cout << cb.C(10, 3) << ' ' << cb.C(100000, 50000) << '\n';  // 120 149033233
    cout << cb.P(10, 3) << ' ' << cb.multinomial({2, 2, 2}) << '\n';  // 720 90
    cout << cb.stars_bars(5, 3) << ' ' << cb.C(3, 5) << '\n';  // 21 0
    cout << binom_big_n(1e18, 5, MOD) << '\n';      // 1906884
    cout << (ll)binom_exact(60, 30) << '\n';        // 118264581564861424
    cout << pascal(10, 6)[10][3] << '\n';           // 0
}
