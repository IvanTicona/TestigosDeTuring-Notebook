/**
 * Usage: Lucas lu(p); lu.C(n, k)  -> C(n, k) mod small prime p, n, k < 2^63
 *        binom_mod(n, k, m)        -> C(n, k) mod ANY m >= 1
 * Complexity: Lucas O(p) build, O(log_p n) query; binom_mod
 *             O(sum of p^e + log n) per call (each p^e of m up to ~1e7)
 * Source: UTP Lucas theorem (statement only); code own. binom_mod: n! with
 *         p-factors removed mod p^e (generalized Lucas), then CRT
 * Note: Lucas: C(n, k) == prod C(n_i, k_i) (mod p), n_i, k_i base-p digits.
 *       For prime p > k use binom_big_n (binomial.cpp) instead.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/number_theory/extended_euclid.cpp
 * Depends: src/number_theory/crt.cpp
 * Depends: src/number_theory/legendre.cpp
 * Depends: src/combinatorics/binomial.cpp
 * Status: untested
 */
struct Lucas {
    ll p;
    Comb cb;
    Lucas(ll p_) : p(p_), cb((int)p_ - 1, p_) {}
    ll C(ll n, ll k) const {
        if (k < 0 || k > n) return 0;
        ll r = 1;
        for (; n || k; n /= p, k /= p) r = r * cb.C(n % p, k % p) % p;
        return r;
    }
};

// C(n, k) mod q = p^e (q = p^e, e >= 1)
ll binom_prime_power(ll n, ll k, ll p, ll q, int e) {
    ll v = vp_fact(n, p) - vp_fact(k, p) - vp_fact(n - k, p);
    if (v >= e) return 0;
    vl pre(q + 1, 1);  // pre[i] = product of 1..i coprime to p, mod q
    for (ll i = 1; i <= q; i++)
        pre[i] = i % p ? pre[i - 1] * i % q : pre[i - 1];
    auto f = [&](ll x) {  // x! without p-factors, mod q
        ll r = 1;
        for (; x; x /= p)
            r = r * powmod(pre[q], x / q, q) % q * pre[x % q] % q;
        return r;
    };
    ll r = f(n) * invmod(f(k), q) % q * invmod(f(n - k), q) % q;
    for (ll i = 0; i < v; i++) r = r * p % q;
    return r;
}

ll binom_mod(ll n, ll k, ll m) {
    if (k < 0 || k > n) return 0;
    vl rs, ms;
    ll t = m;
    for (ll p = 2; p * p <= t; p++)
        if (t % p == 0) {
            ll q = 1; int e = 0;
            while (t % p == 0) t /= p, q *= p, e++;
            rs.pb(binom_prime_power(n, k, p, q, e)), ms.pb(q);
        }
    if (t > 1) rs.pb(binom_prime_power(n, k, t, t, 1)), ms.pb(t);
    return crt(rs, ms).F % m;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_lucas() {
    Lucas l7(7), l13(13);
    // 7^20 - 1 has all base-7 digits 6: C(n, k) is never 0 mod 7
    cout << l7.C(79792266297612000, 123456789) << '\n';  // 6
    cout << l13.C(1000, 500) << '\n';                    // 0
    cout << binom_mod(1000, 400, 142857) << '\n';        // 65934
    cout << binom_mod(1000, 400, 1 << 20) << '\n';       // 409934
    cout << binom_mod(2000, 1000, 720720) << '\n';       // 320320
}
