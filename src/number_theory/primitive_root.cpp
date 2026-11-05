/**
 * Usage: ll g = primitive_root(p);   // smallest generator of (Z/pZ)*
 *        ll k = mult_order(a, m);    // least k > 0: a^k == 1 (mod m)
 * Complexity: O(sqrt(m)) to factor, then O(#primes * log^2 m) checks
 * Source: standard (cp-algorithms); not in the reference notebooks
 * Note: p prime; mult_order needs gcd(a, m) == 1 (returns -1 otherwise).
 *       Trial division: fine for m up to ~1e12; beyond, factor with
 *       Pollard rho.
 *       g is a generator iff g^((p-1)/q) != 1 for every prime q | p - 1.
 *       Known roots: 998244353 -> 3, 1e9+7 -> 5, 2^31-1 -> 7.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: ok
 */
vl distinct_prime_factors(ll n) {  // distinct primes, trial division
    vl f;
    for (ll d = 2; d * d <= n; d++)
        if (n % d == 0) { f.pb(d); while (n % d == 0) n /= d; }
    if (n > 1) f.pb(n);
    return f;
}

ll primitive_root(ll p) {
    if (p == 2) return 1;
    vl qs = distinct_prime_factors(p - 1);
    for (ll g = 2;; g++) {
        bool ok = true;
        for (ll q : qs)
            if (powmod(g, (p - 1) / q, p) == 1) { ok = false; break; }
        if (ok) return g;
    }
}

ll mult_order(ll a, ll m) {
    a %= m; if (a < 0) a += m;
    if (gcd(a, m) != 1) return -1;
    ll phi = m;  // Euler phi by trial division
    for (ll q : distinct_prime_factors(m)) phi -= phi / q;
    ll k = phi;  // order divides phi: strip prime factors while possible
    for (ll q : distinct_prime_factors(phi))
        while (k % q == 0 && powmod(a, k / q, m) == 1 % (ull)m) k /= q;
    return k;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_primitive_root() {
    cout << primitive_root(998244353) << '\n';      // 3
    cout << primitive_root(MOD) << ' ' << primitive_root(7) << '\n';  // 5 3
    cout << mult_order(2, 7) << ' ' << mult_order(3, 7) << '\n';  // 3 6
    cout << mult_order(2, 9) << ' ' << mult_order(3, 9) << '\n';  // 6 -1
}
