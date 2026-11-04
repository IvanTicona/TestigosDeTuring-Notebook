/**
 * Usage: is_prime(n) -> deterministic for every n < 2^63
 *        factor(n)   -> prime factors with multiplicity, sorted
 *        factor_pe(n) -> {prime, exponent} pairs
 * Complexity: is_prime O(7 log n); factor ~O(n^(1/4)) expected per split
 *             (1e18 semiprime: well under 1 ms)
 * Source: UMSS Optimized Pollard Rho (structure) + KACTL Pollard (Brent,
 *         deterministic, batched gcd); Miller-Rabin bases from Jim Sinclair
 * Note: UMSS bases {2..23} are only valid below 3.8e18
 *       (3825123056546413051 is a strong pseudoprime to all of them) and
 *       UPB bases {2..17} fail at 341550071728321; the 7 bases below are
 *       deterministic for all 64-bit n.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: untested
 */
bool is_prime(ll n) {
    if (n < 2) return false;
    for (ll p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37})
        if (n % p == 0) return n == p;
    ull d = n - 1;
    int s = __builtin_ctzll(d);
    d >>= s;
    for (ull a : {2, 325, 9375, 28178, 450775, 9780504, 1795265022}) {
        ull x = powmod(a, d, n);
        if (a % n == 0 || x == 1 || x == (ull)n - 1) continue;
        for (int r = 1; r < s && x != (ull)n - 1; r++) x = mulmod(x, x, n);
        if (x != (ull)n - 1) return false;  // a witnesses n composite
    }
    return true;
}

// a non-trivial factor of an odd composite n (Pollard-Brent)
ll pollard(ll n) {
    ull x = 0, y = 0, t = 30, prd = 2, i = 1, q;
    auto f = [&](ull v) { return mulmod(v, v, n) + i; };
    while (t++ % 40 || gcd(prd, (ull)n) == 1) {
        if (x == y) x = ++i, y = f(x);
        if ((q = mulmod(prd, max(x, y) - min(x, y), n))) prd = q;
        x = f(x), y = f(f(y));
    }
    return gcd(prd, (ull)n);
}

vl factor(ll n) {
    vl res;
    for (ll p : {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37})
        while (n % p == 0) res.pb(p), n /= p;
    vl st = {n};  // remaining parts to split
    while (!st.empty()) {
        ll m = st.back(); st.pop_back();
        if (m == 1) continue;
        if (is_prime(m)) { res.pb(m); continue; }
        ll d = pollard(m);
        st.pb(d), st.pb(m / d);
    }
    sort(all(res));
    return res;
}

vector<pll> factor_pe(ll n) {
    vector<pll> pe;
    for (ll p : factor(n))
        if (!pe.empty() && pe.back().F == p) pe.back().S++;
        else pe.pb({p, 1});
    return pe;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_pollard_rho() {
    cout << is_prime(1e9 + 7) << is_prime(341550071728321) << '\n';  // 10
    cout << is_prime(3825123056546413051) << '\n';  // 0
    string s;
    for (ll p : factor(999999943999999559)) s += to_string(p) + " ";
    cout << s << '\n';                              // 999999937 1000000007
    s = "";
    for (auto [p, e] : factor_pe(720720))
        s += to_string(p) + "^" + to_string(e) + " ";
    cout << s << '\n';                              // 2^4 3^2 5^1 7^1 11^1 13^1
}
