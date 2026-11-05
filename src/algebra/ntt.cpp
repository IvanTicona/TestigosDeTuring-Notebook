/**
 * Usage: ntt_mul<P, G>(a, b)  -> a * b mod NTT prime P (default 998244353)
 *        ntt<P, G>(a, inv)    -> in-place transform, sz(a) power of 2
 *        conv_any(a, b, m)    -> a * b mod ANY m < 2^31 (3 NTT primes + CRT)
 * Complexity: O(n log n); conv_any = 3 products
 * Source: UPB NTT (998244353) + UPB isc-notebook math_ntt (1004535809) +
 *         UMSS NTT (2305843009255636993, i128 mulmod, global arrays,
 *         MAXN fixed) + UTP number theoretic transform; structure KACTL
 * Note: P = c * 2^k + 1 allows sizes up to 2^k. G = primitive root of P.
 *       998244353 = 119*2^23+1 (G 3)   167772161 = 5*2^25+1 (G 3)
 *       469762049 = 7*2^26+1 (G 3)     754974721 = 45*2^24+1 (G 11)
 *       Inputs must be in [0, P). conv_any is exact while
 *       n * (m-1)^2 < P1*P2*P3 ~ 7.9e25 (n = min size).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: ok
 */
template <ll P = MOD2, ll G = 3>
void ntt(vl &a, bool inv = false) {
    int n = sz(a), L = 31 - __builtin_clz(n);
    static vl rt(2, 1);  // rt[k + j] = w_{2k}^j
    for (static int k = 2, s = 2; k < n; k *= 2, s++) {
        rt.resize(n);
        ll z[] = {1, (ll)powmod(G, (P - 1) >> s, P)};
        for (int i = k; i < 2 * k; i++) rt[i] = rt[i / 2] * z[i & 1] % P;
    }
    vi rev(n);
    for (int i = 0; i < n; i++) rev[i] = (rev[i / 2] | (i & 1) << L) / 2;
    for (int i = 0; i < n; i++) if (i < rev[i]) swap(a[i], a[rev[i]]);
    for (int k = 1; k < n; k *= 2)
        for (int i = 0; i < n; i += 2 * k)
            for (int j = 0; j < k; j++) {
                ll z = rt[j + k] * a[i + j + k] % P, &ai = a[i + j];
                a[i + j + k] = ai - z + (z > ai ? P : 0);
                ai += (ai + z >= P ? z - P : z);
            }
    if (inv) {  // inverse = forward + reverse a[1..n-1] + divide by n
        reverse(a.begin() + 1, a.end());
        ll in = powmod(n, P - 2, P);
        for (ll &x : a) x = x * in % P;
    }
}

template <ll P = MOD2, ll G = 3>
vl ntt_mul(vl a, vl b) {
    if (a.empty() || b.empty()) return {};
    int s = sz(a) + sz(b) - 1, n = 1;
    while (n < s) n *= 2;
    a.resize(n), b.resize(n);
    ntt<P, G>(a), ntt<P, G>(b);
    for (int i = 0; i < n; i++) a[i] = a[i] * b[i] % P;
    ntt<P, G>(a, true);
    a.resize(s);
    return a;
}

vl conv_any(vl a, vl b, ll m) {
    const ll P1 = 998244353, P2 = 167772161, P3 = 469762049;
    for (ll &x : a) x %= m;
    for (ll &x : b) x %= m;
    vl c1 = ntt_mul<P1>(a, b), c2 = ntt_mul<P2>(a, b), c3 = ntt_mul<P3>(a, b);
    const ll i12 = powmod(P1, P2 - 2, P2);
    const ll i123 = powmod(P1 * P2 % P3, P3 - 2, P3);
    vl r(sz(c1));
    for (int i = 0; i < sz(r); i++) {  // Garner: x = c1 + P1 k1 + P1 P2 k2
        ll k1 = (c2[i] - c1[i] % P2 + P2) % P2 * i12 % P2;
        ll x12 = c1[i] + P1 * k1;  // < P1 P2 ~ 1.7e17
        ll k2 = (c3[i] - x12 % P3 + P3) % P3 * i123 % P3;
        r[i] = (x12 % m + (ll)mulmod(P1 * P2 % m, k2, m)) % m;
    }
    return r;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_ntt() {
    vl c = ntt_mul({1, 2, 3}, {MOD2 - 1, 1});  // (1+2x+3x^2)(x-1)
    cout << c[0] << ' ' << c[1] << ' '  // 998244352 998244352 998244352 3
         << c[2] << ' ' << c[3] << '\n';
    vl d = conv_any({MOD - 1, MOD - 1}, {MOD - 1}, MOD);  // (-1-x)(-1)
    cout << d[0] << ' ' << d[1] << '\n';                 // 1 1
    vl e = ntt_mul<167772161>({5}, {7});
    cout << e[0] << '\n';                                // 35
}
