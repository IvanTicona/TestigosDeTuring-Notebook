/**
 * Usage: powmod_str(a, "123456...", m) -> a^E mod m, E a huge decimal string
 *        tower({a1, a2, ..., ak}, m)    -> a1^(a2^(...^ak)) mod m (ai >= 1)
 * Complexity: powmod_str O(|E| + sqrt(m)); tower O(k sqrt(m)) (phi chain)
 * Source: UMSS Big Exponent (a^(b^c) mod prime) generalized; own
 * Note: generalized Euler: for ANY m, if E >= phi(m) (>= log2 m) then
 *       a^E == a^(E mod phi(m) + phi(m)) (mod m), even if gcd(a, m) > 1.
 *       If E < phi(m) use E directly. UMSS reduced mod p-1 always and
 *       printed 1 instead of 0 when p | a and (p-1) | b^c.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/number_theory/multiplicative_functions.cpp
 * Status: untested
 */
ll powmod_str(ll a, const string &E, ll m) {
    ll ph = phi(m), r = 0;
    bool big = false;  // E >= ph ?
    for (char ch : E) {
        __int128 v = (__int128)r * 10 + (ch - '0');
        if (v >= ph) big = true;
        r = (ll)(v % ph);
    }
    a %= m; if (a < 0) a += m;
    return powmod(a, big ? r + ph : r, m);
}

// "lifted" residue: x if x < m, else x % m + m (remembers that x >= m)
ll lift(__int128 x, ll m) { return x < m ? (ll)x : (ll)(x % m + m); }
ll pow_lift(ll a, ll e, ll m) {  // a >= 1, e lifted mod phi(m)
    ll r = lift(1, m);
    for (a = lift(a, m); e; e >>= 1, a = lift((__int128)a * a, m))
        if (e & 1) r = lift((__int128)r * a, m);
    return r;
}
ll tower_lift(const vl &a, int i, ll m) {
    if (i == sz(a)) return lift(1, m);  // empty tower = 1
    if (m == 1) return 1;               // value >= 1, lifted mod 1
    return pow_lift(a[i], tower_lift(a, i + 1, phi(m)), m);
}
ll tower(const vl &a, ll m) { return tower_lift(a, 0, m) % m; }

// ---- Example: call from solve(); each // is the line it prints ----
void example_huge_exponent() {
    string E = "1" + string(100, '0');              // 10^100
    cout << powmod_str(2, E, MOD) << '\n';          // 314344290
    // 2^(3^4) = 2^81; UMSS printed 1 for the next one; 2^2^2^2^2 = 2^65536
    cout << tower({2, 3, 4}, 1000) << '\n';         // 352
    cout << tower({MOD, MOD - 1, 1}, MOD) << '\n';  // 0
    cout << tower({2, 2, 2, 2, 2}, 1000000) << '\n';  // 156736
}
