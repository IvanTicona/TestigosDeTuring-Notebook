/**
 * Usage: lagrange_seq(y, x, p)    -> P(x) mod p, where deg P <= n and
 *                                   y[i] = P(i) for i = 0..n; x up to 1e18
 *        interpolate(xs, ys, p)   -> coefficients c (P(x) = sum c_i x^i) of
 *                                   the deg < n poly with P(xs[i]) = ys[i]
 *        power_sum(N, k, p)       -> 1^k + 2^k + ... + N^k mod p, N ~ 1e18
 * Complexity: lagrange_seq O(n log p); interpolate O(n^2); power_sum O(k log)
 * Source: own (not in the references)
 * Note: p prime, p > n (lagrange_seq, power_sum: p > k + 1); xs distinct
 *       mod p. Any answer "polynomial in N of degree d" (sums of powers,
 *       some DP counts) -> compute d + 1 values by brute force, then
 *       lagrange_seq. P(x) mod p depends only on x mod p (p > deg).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Status: untested
 */
ll lagrange_seq(const vl &y, ll x, ll p = MOD) {
    int n = sz(y) - 1;
    x %= p;
    if (x < 0) x += p;
    if (x <= n) return y[x] % p;
    vl pre(n + 2, 1), suf(n + 2, 1), fi(n + 1, 1);  // fi = 1/i!
    for (int i = 0; i <= n; i++) pre[i + 1] = pre[i] * ((x - i) % p) % p;
    for (int i = n; i >= 0; i--) suf[i] = suf[i + 1] * ((x - i) % p) % p;
    ll f = 1;
    for (int i = 1; i <= n; i++) f = f * i % p;
    fi[n] = invmod_prime(f, p);
    for (int i = n; i > 0; i--) fi[i - 1] = fi[i] * i % p;
    ll r = 0;
    for (int i = 0; i <= n; i++) {  // y_i prod_{j!=i} (x-j)/(i-j)
        ll t = y[i] % p * pre[i] % p * suf[i + 1] % p;
        t = t * fi[i] % p * fi[n - i] % p;
        r = (n - i) & 1 ? (r - t + p) % p : (r + t) % p;
    }
    return r;
}

vl interpolate(const vl &xs, const vl &ys, ll p = MOD) {
    int n = sz(xs);
    vl M(n + 1, 0), c(n, 0);  // M = prod (x - xs[i])
    M[0] = 1;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j >= 0; j--)
            M[j] = ((j ? M[j - 1] : 0) + (p - xs[i] % p) * M[j]) % p;
    for (int i = 0; i < n; i++) {
        vl q(n);  // q = M / (x - xs[i]), synthetic division
        ll carry = 0, d = 1, xi = xs[i] % p;
        for (int j = n; j >= 1; j--)
            carry = (M[j] + carry * xi) % p, q[j - 1] = carry;
        for (int j = 0; j < n; j++)
            if (j != i) d = d * ((xi - xs[j] % p + p) % p) % p;
        ll coef = ys[i] % p * invmod_prime(d, p) % p;
        for (int j = 0; j < n; j++) c[j] = (c[j] + coef * q[j]) % p;
    }
    return c;
}

ll power_sum(ll N, int k, ll p = MOD) {  // deg k + 1 -> k + 2 points
    vl y(k + 2, 0);
    for (int i = 1; i <= k + 1; i++) y[i] = (y[i - 1] + powmod(i, k, p)) % p;
    return lagrange_seq(y, N, p);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_lagrange() {
    // y = sums of squares 0, 1, 5, 14 -> value at 10
    cout << lagrange_seq({0, 1, 5, 14}, 10) << '\n';     // 385
    // 1 + ... + 1e6 = 500000500000, reduced mod 1e9+7
    cout << power_sum(1000000, 1) << '\n';               // 496500
    cout << power_sum(100, 3) << '\n';                   // 25502500
    vl c = interpolate({1, 2, 3}, {6, 11, 18});          // x^2 + 2x + 3
    cout << c[0] << ' ' << c[1] << ' ' << c[2] << '\n';  // 3 2 1
}
