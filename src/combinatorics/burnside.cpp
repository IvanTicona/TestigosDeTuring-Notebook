/**
 * Usage: necklaces(n, k)          -> exact #colorings of an n-cycle with k
 *                                   colors up to rotation (__int128)
 *        necklaces_mod(n, k, p)   -> same mod prime p (n < p), n up to ~1e12
 *        bracelets(n, k)          -> up to rotation AND reflection (exact)
 *        burnside(cycles, k)      -> Polya: (1/|G|) sum_g k^cycles(g), exact;
 *                                   cycles[i] = #cycles of the i-th symmetry
 * Complexity: necklaces O(sqrt n * d(n)) (phi of every divisor)
 * Source: own (Burnside's lemma / Polya enumeration; not in the references)
 * Note: Burnside: #orbits = (1/|G|) sum_g |Fix(g)|. A rotation by i of an
 *       n-cycle has gcd(i, n) cycles, and exactly phi(d) rotations have
 *       gcd = n/d  =>  necklaces = (1/n) sum_{d | n} phi(d) k^(n/d).
 *       bracelets = necklaces/2 + (n odd ? k^((n+1)/2) / 2
 *                                         : (k+1) k^(n/2) / 4).
 *       Cube faces, 24 rotations: cycles 6, 3 x6, 4 x3, 2 x8, 3 x6.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/number_theory/multiplicative_functions.cpp
 * Status: untested
 */
i128 ipow128(i128 b, ll e) { i128 r = 1; while (e--) r *= b; return r; }

i128 necklaces(ll n, ll k) {  // exact while k^n fits in __int128
    i128 s = 0;
    for (ll d = 1; d * d <= n; d++)
        if (n % d == 0) {
            s += phi(d) * ipow128(k, n / d);
            if (d != n / d) s += phi(n / d) * ipow128(k, d);
        }
    return s / n;
}

ll necklaces_mod(ll n, ll k, ll p) {
    ll s = 0;
    for (ll d = 1; d * d <= n; d++)
        if (n % d == 0) {
            s = (s + (ll)mulmod(phi(d) % p, powmod(k, n / d, p), p)) % p;
            if (d != n / d)
                s = (s + (ll)mulmod(phi(n / d) % p, powmod(k, d, p), p)) % p;
        }
    return (ll)mulmod(s, invmod_prime(n % p, p), p);
}

i128 bracelets(ll n, ll k) {
    i128 N = necklaces(n, k);
    if (n % 2) return (N + ipow128(k, (n + 1) / 2)) / 2;
    return (N + (k + 1) * ipow128(k, n / 2) / 2) / 2;
}

i128 burnside(const vl &cycles, ll k) {
    i128 s = 0;
    for (ll c : cycles) s += ipow128(k, c);
    return s / sz(cycles);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_burnside() {
    cout << (ll)necklaces(4, 2) << ' ' << (ll)necklaces(6, 3) << '\n';  // 6 130
    cout << (ll)bracelets(6, 3) << '\n';            // 92
    cout << (ll)bracelets(10, 2) << '\n';           // 78
    cout << (ll)necklaces(10, 2) << '\n';           // 108
    cout << necklaces_mod(1e9, 2, MOD) << '\n';     // 388199333
    vl cube = {6, 3, 3, 3, 3, 3, 3, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2,
               3, 3, 3, 3, 3, 3};
    cout << (ll)burnside(cube, 3) << '\n';          // 57
}
