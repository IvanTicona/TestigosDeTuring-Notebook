/**
 * Usage: ll r = sqrtmod(a, p);  // r*r == a (mod p), or -1 if a is not a
 *        quadratic residue. The other root is p - r.
 * Complexity: O(log^2 p)
 * Source: Tonelli-Shanks (standard; not in the reference notebooks)
 * Note: p must be an odd prime or 2.
 *       Euler's criterion: a is a residue iff a^((p-1)/2) == 1 (mod p).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: untested
 */
ll sqrtmod(ll a, ll p) {
    a %= p; if (a < 0) a += p;
    if (a == 0 || p == 2) return a;
    if (powmod(a, (p - 1) / 2, p) != 1) return -1;  // not a residue
    if (p % 4 == 3) return powmod(a, (p + 1) / 4, p);
    ll q = p - 1, s = 0, z = 2;
    while (q % 2 == 0) q /= 2, s++;                    // p - 1 = q * 2^s
    while (powmod(z, (p - 1) / 2, p) != (ull)p - 1) z++;  // non-residue
    ll m = s, c = powmod(z, q, p), t = powmod(a, q, p);
    ll r = powmod(a, (q + 1) / 2, p);
    while (t != 1) {
        ll i = 0, t2 = t;
        while (t2 != 1) t2 = mulmod(t2, t2, p), i++;   // least i: t^(2^i) == 1
        ll b = powmod(c, 1ULL << (m - i - 1), p);
        m = i, c = mulmod(b, b, p);
        t = mulmod(t, c, p), r = mulmod(r, b, p);
    }
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_modular_sqrt() {
    ll r = sqrtmod(10, 13);                         // 6^2 = 36 = 10 (mod 13)
    cout << min(r, 13 - r) << ' ' << max(r, 13 - r) << '\n';  // 6 7
    cout << sqrtmod(5, 13) << '\n';                 // -1
    r = sqrtmod(2, MOD);
    cout << mulmod(r, r, MOD) << '\n';              // 2
}
