/**
 * Usage: auto [x, l] = crt({a1, a2, ...}, {m1, m2, ...});
 *          -> x == ai (mod mi) for all i, unique mod l = lcm(mi);
 *             {-1, -1} if there is no solution. Moduli need NOT be coprime.
 *        ll r = garner({a1, ...}, {m1, ...}, MODO);  // x mod MODO
 *          -> mi pairwise coprime; x itself may be huge (> 2^64)
 * Complexity: O(k log max m) for k congruences; garner O(k^2 log m)
 * Source: UPB CRT_Garner (base); UMSS crt overflowed for m ~ 1e9
 * Note: crt needs l = lcm(mi) <= ~9.2e18; if it can be larger, use garner.
 *       Typical use of garner: combine 3 NTT primes to get a result mod 1e9+7.
 * Depends: src/number_theory/extended_euclid.cpp
 * Status: untested
 */
// merge x == a1 (mod m1) and x == a2 (mod m2) into x == a (mod lcm)
pair<ll, ll> crt_merge(ll a1, ll m1, ll a2, ll m2) {
    a1 %= m1; if (a1 < 0) a1 += m1;
    a2 %= m2; if (a2 < 0) a2 += m2;
    ll p, q, g = extgcd(m1, m2, p, q);  // m1*p + m2*q = g
    if ((a2 - a1) % g) return {-1, -1};
    ll m2g = m2 / g;
    // a1 + m1*k == a2 (mod m2)  ->  k == (a2 - a1)/g * p (mod m2/g)
    ll k = (ll)((__int128)((a2 - a1) / g) * p % m2g);
    if (k < 0) k += m2g;
    __int128 l = (__int128)m1 * m2g;
    return {(ll)((a1 + (__int128)m1 * k) % l), (ll)l};
}

pair<ll, ll> crt(const vl &a, const vl &m) {
    pair<ll, ll> r = {0, 1};  // x == 0 (mod 1)
    for (int i = 0; i < sz(a); i++) {
        r = crt_merge(r.F, r.S, a[i], m[i]);
        if (r.S == -1) return {-1, -1};
    }
    return r;
}

// x == a[i] (mod m[i]), m pairwise coprime; returns x mod md (md >= 1)
ll garner(const vl &a, const vl &m, ll md) {
    int k = sz(a);
    vl mm = m; mm.pb(md);
    vl coef(k + 1, 1), cst(k + 1, 0);  // value so far mod mm[i]
    for (int i = 0; i < k; i++) {
        ll inv, tmp;
        extgcd(coef[i] % mm[i], mm[i], inv, tmp);  // coef * inv == 1
        inv %= mm[i]; if (inv < 0) inv += mm[i];
        ll t = ((a[i] - cst[i]) % mm[i] + mm[i]) % mm[i];
        ll d = (ll)((__int128)t * inv % mm[i]);  // next mixed-radix digit
        for (int j = i + 1; j <= k; j++) {
            cst[j] = (ll)((cst[j] + (__int128)coef[j] * d) % mm[j]);
            coef[j] = (ll)((__int128)coef[j] * mm[i] % mm[j]);
        }
    }
    return cst[k];
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_crt() {
    auto [x, l] = crt({2, 3, 2}, {3, 5, 7});
    cout << x << ' ' << l << '\n';                  // 23 105
    auto r = crt({3, 5}, {4, 6});                   // non-coprime moduli
    cout << r.F << ' ' << r.S << '\n';              // 11 12
    r = crt({1, 2}, {2, 4});                        // contradictory
    cout << r.F << ' ' << r.S << '\n';              // -1 -1
    // x = 123456789012345678901234567 > 2^64, known mod three primes
    vl res = {294645637, 864197440, 790123284};
    vl ps = {998244353, 1000000007, 1000000009};
    cout << garner(res, ps, (ll)1e18) << '\n';      // 12345678901234567
}
