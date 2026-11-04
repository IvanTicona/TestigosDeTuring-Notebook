/**
 * Usage: karatsuba(a, b, m) -> a * b mod ANY m < 2^31 (coefs in [0, m))
 * Complexity: O(n^1.585); ~1.3 s for n = 1e5 (FFT / NTT are faster)
 * Source: UMSS Karatsuba (raw new[] released with free(): undefined
 *         behavior; int buffers), rewritten with vectors
 * Note: exact integer arithmetic, no precision issues (vs FFT) and no
 *       prime needed (vs NTT). Prefer conv_mod / conv_any for n > ~5e4.
 * Status: untested
 */
vl kara_rec(const vl &a, const vl &b, ll m) {  // sz(a) == sz(b)
    int n = sz(a);
    vl r(2 * n - 1, 0);
    if (n <= 64) {  // keep sums < 2 m^2 < 2^63, reduce once at the end
        ll M2 = m * m;
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) {
                r[i + j] += a[i] * b[j];
                if (r[i + j] >= M2) r[i + j] -= M2;
            }
        for (ll &x : r) x %= m;
        return r;
    }
    int h = n / 2;
    vl a0(a.begin(), a.begin() + h), a1(a.begin() + h, a.end());
    vl b0(b.begin(), b.begin() + h), b1(b.begin() + h, b.end());
    vl z0 = kara_rec(a0, b0, m), z2 = kara_rec(a1, b1, m);
    a0.resize(n - h), b0.resize(n - h);
    for (int i = 0; i < n - h; i++)
        a0[i] = (a0[i] + a1[i]) % m, b0[i] = (b0[i] + b1[i]) % m;
    vl z1 = kara_rec(a0, b0, m);  // (a0 + a1)(b0 + b1)
    for (int i = 0; i < sz(z0); i++)
        r[i] = (r[i] + z0[i]) % m, z1[i] = (z1[i] - z0[i] + m) % m;
    for (int i = 0; i < sz(z2); i++) {
        r[i + 2 * h] = (r[i + 2 * h] + z2[i]) % m;
        z1[i] = (z1[i] - z2[i] + m) % m;
    }
    for (int i = 0; i < sz(z1); i++) r[i + h] = (r[i + h] + z1[i]) % m;
    return r;
}

vl karatsuba(vl a, vl b, ll m) {
    if (a.empty() || b.empty()) return {};
    int s = sz(a) + sz(b) - 1, n = max(sz(a), sz(b));
    a.resize(n), b.resize(n);
    vl r = kara_rec(a, b, m);
    r.resize(s);
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_karatsuba() {
    vl c = karatsuba({1, 2, 3}, {4, 5}, 10);  // 4 13 22 15 mod 10
    cout << c[0] << ' ' << c[1] << ' ' << c[2] << ' ' << c[3]  // 4 3 2 5
         << '\n';
    vl d = karatsuba(vl(100, 1), vl(100, 1), MOD);  // coef k = #(i + j = k)
    cout << d[0] << ' ' << d[99] << ' ' << d[198] << '\n';  // 1 100 1
}
