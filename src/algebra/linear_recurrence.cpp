/**
 * Usage: berlekamp_massey(s, p) -> shortest c with s_i = sum_j c[j] s_{i-1-j}
 *        kitamasa(c, f, n, p)   -> f_n of that recurrence (f = first sz(c)
 *                                  terms), n up to 1e18
 *        guess_nth(s, n, p)     -> both: give ~2k+10 first terms, get s_n
 * Complexity: BM O(N^2); kitamasa O(k^2 log n)
 * Source: UMSS Linear Recurrence Cayley-Hamilton (latent compile error:
 *         modint has no *=) + UPB Kitamasa_BerlekampMassey (inverse via
 *         floating pow(b, MOD-2): WRONG; e[1 % k] wrong for k = 1);
 *         structure KACTL
 * Note: p prime < 2^31, terms in [0, p). BM needs 2k terms for order k:
 *       feed generously (any DP whose state vector evolves linearly, e.g.
 *       #paths in a fixed graph, counts with small transfer matrices).
 *       Kitamasa = x^n mod the characteristic polynomial (Cayley-Hamilton).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: ok
 */
vl berlekamp_massey(const vl &s, ll p = MOD) {
    int n = sz(s), L = 0, m = 0;
    vl C(n + 1, 0), B(n + 1, 0), T;
    C[0] = B[0] = 1;
    ll b = 1;
    for (int i = 0; i < n; i++) {
        m++;
        ll d = s[i] % p;
        for (int j = 1; j <= L; j++) d = (d + C[j] * s[i - j]) % p;
        if (!d) continue;
        T = C;
        ll coef = d * invmod_prime(b, p) % p;
        for (int j = m; j <= n; j++)
            C[j] = ((C[j] - coef * B[j - m]) % p + p) % p;
        if (2 * L > i) continue;
        L = i + 1 - L, B = T, b = d, m = 0;
    }
    C.resize(L + 1);
    C.erase(C.begin());
    for (ll &x : C) x = (p - x) % p;
    return C;
}

ll kitamasa(const vl &c, const vl &f, ll n, ll p = MOD) {
    int k = sz(c);
    if (k == 0) return 0;
    if (n < k) return f[n] % p;
    auto combine = [&](const vl &a, const vl &b) {  // a * b mod charpoly
        vl r(2 * k + 1, 0);
        for (int i = 0; i <= k; i++)
            for (int j = 0; j <= k; j++)
                r[i + j] = (r[i + j] + a[i] * b[j]) % p;
        for (int i = 2 * k; i > k; i--)
            for (int j = 0; j < k; j++)
                r[i - 1 - j] = (r[i - 1 - j] + r[i] * c[j]) % p;
        r.resize(k + 1);
        return r;
    };
    vl pol(k + 1, 0), e(k + 1, 0);  // shifted by one: index i <-> x^(i-1)
    pol[0] = e[1] = 1;
    for (++n; n; n /= 2) {
        if (n % 2) pol = combine(pol, e);
        e = combine(e, e);
    }
    ll r = 0;
    for (int i = 0; i < k; i++) r = (r + pol[i + 1] * f[i]) % p;
    return r;
}

ll guess_nth(const vl &s, ll n, ll p = MOD) {
    if (n < sz(s)) return s[n] % p;
    return kitamasa(berlekamp_massey(s, p), s, n, p);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_linear_recurrence() {
    vl c = berlekamp_massey({0, 1, 1, 2, 3, 5, 8, 13});
    cout << sz(c) << ' ' << c[0] << ' ' << c[1] << '\n';  // 2 1 1
    cout << kitamasa({1, 1}, {0, 1}, 90) << '\n';        // 210345902
    // squares 0 1 4 9 ... are a linear recurrence of order 3
    vl sq = {0, 1, 4, 9, 16, 25, 36, 49};
    cout << guess_nth(sq, 1000000) << '\n';  // 999993007
}
