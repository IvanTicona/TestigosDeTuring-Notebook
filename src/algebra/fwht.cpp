/**
 * Usage: bit_conv(a, b, op, p) -> c[k] = sum over i op j == k of a[i] b[j]
 *                                 op: 0 XOR, 1 AND, 2 OR; p = 0 -> exact
 *        fwht(a, op, inv, p)   -> in-place transform, sz(a) power of 2
 *        subset_conv(f, g, p)  -> h[S] = sum_{A | B = S, A & B = 0} f[A] g[B]
 * Complexity: bit_conv O(n log n); subset_conv O(n^2 2^n), N = 2^n
 * Source: UMSS Fast Hadamard Transform (one op per edit, global arrays,
 *         exact only) + UPB InverseSOS_SubsetConvolution (exact ll only)
 * Note: OR transform = subset sums (zeta), AND = superset sums; their
 *       inverses are the Mobius transforms (SOS DP). XOR inverse divides by
 *       n: p must be an odd prime (or p = 0 with exact values).
 *       c[0] of XOR self-convolution counts pairs with a[i] == a[j]; to
 *       count pairs i < j from a frequency array: (c[k] - [k == 0] n) / 2.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: untested
 */
void fwht(vl &a, int op, bool inv, ll p = MOD) {
    int n = sz(a);
    auto md = [&](ll x) { return p ? (x % p + p) % p : x; };
    for (int k = 1; k < n; k *= 2)
        for (int i = 0; i < n; i += 2 * k)
            for (int j = i; j < i + k; j++) {
                ll &u = a[j], &v = a[j + k];
                if (op == 0) { ll x = u, y = v; u = md(x + y), v = md(x - y); }
                else if (op == 1) u = md(inv ? u - v : u + v);
                else v = md(inv ? v - u : v + u);
            }
    if (op == 0 && inv) {
        if (p) { ll in = invmod_prime(n % p, p); for (ll &x : a) x = x * in % p; }
        else for (ll &x : a) x /= n;
    }
}

vl bit_conv(vl a, vl b, int op, ll p = MOD) {
    int n = 1;
    while (n < max(sz(a), sz(b))) n *= 2;
    a.resize(n), b.resize(n);
    fwht(a, op, false, p), fwht(b, op, false, p);
    for (int i = 0; i < n; i++) a[i] = p ? a[i] % p * (b[i] % p) % p : a[i] * b[i];
    fwht(a, op, true, p);
    return a;
}

vl subset_conv(const vl &f, const vl &g, ll p = MOD) {  // sz = 2^n, p > 0
    int N = sz(f), n = __builtin_ctz(N);
    vector<vl> F(n + 1, vl(N, 0)), G = F, H = F;
    for (int m = 0; m < N; m++) {
        int c = __builtin_popcount(m);
        F[c][m] = f[m] % p, G[c][m] = g[m] % p;
    }
    for (int k = 0; k <= n; k++) fwht(F[k], 2, false, p), fwht(G[k], 2, false, p);
    for (int m = 0; m < N; m++)
        for (int k = 0; k <= n; k++)
            for (int i = 0; i <= k; i++) H[k][m] = (H[k][m] + F[i][m] * G[k - i][m]) % p;
    for (int k = 0; k <= n; k++) fwht(H[k], 2, true, p);
    vl h(N);
    for (int m = 0; m < N; m++) h[m] = H[__builtin_popcount(m)][m];
    return h;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_fwht() {
    vl cnt = {0, 1, 1, 1};  // multiset {1, 2, 3}, cnt[v] = #v
    vl x = bit_conv(cnt, cnt, 0, 0);  // ordered pairs by XOR value
    cout << x[0] << ' ' << x[1] << ' ' << x[2] << ' ' << x[3] << '\n';  // 3 2 2 2
    vl o = bit_conv(cnt, cnt, 2);     // ordered pairs by OR value
    cout << o[0] << ' ' << o[1] << ' ' << o[2] << ' ' << o[3] << '\n';  // 0 1 1 7
    vl h = subset_conv({1, 1, 1, 1}, {1, 1, 1, 1});  // splits of S into A, B
    cout << h[0] << ' ' << h[1] << ' ' << h[3] << '\n';  // 1 2 4
}
