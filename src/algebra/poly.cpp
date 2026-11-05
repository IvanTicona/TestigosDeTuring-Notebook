/**
 * Usage: all mod 998244353 (MOD2), coefficient vectors a[0] + a[1] x + ...
 *        poly_inv(a, n)     -> 1/a mod x^n            (a[0] != 0)
 *        poly_divmod(a, b)  -> {q, r}: a = q b + r, |r| = |b|-1 (b.back()!=0)
 *        poly_log(a, n)     -> ln a mod x^n           (a[0] == 1)
 *        poly_exp(a, n)     -> e^a mod x^n            (a[0] == 0)
 *        poly_pow(a, k, n)  -> a^k mod x^n, any a, k >= 0 (0^0 = 1)
 *        poly_eval(a, xs)   -> {a(x) for x in xs} (multipoint)
 * Complexity: inv/log/exp/pow O(n log n) (exp ~ 10 products);
 *             divmod O(n log n); eval O(n log^2 n)
 * Source: own (Newton iteration on NTT); references only define
 *         polynomials (UMSS Poly Definitions, UTP polynomials)
 * Note: Newton: inv b <- b(2 - a b); exp b <- b(1 - ln b + a), doubling.
 *       Counting uses: 1/(1-x)^k, partitions = prod 1/(1-x^i) = 1/P(x)
 *       (pentagonal), sets of labeled structures = exp(EGF), sequences =
 *       1/(1-A), subset sum counts of distinct items = exp(sum ln(1+x^w)).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/algebra/ntt.cpp
 * Status: ok
 */
vl poly_inv(const vl &a, int n) {
    vl b = {invmod_prime(a[0], MOD2)};
    for (int k = 1; k < n; k *= 2) {
        vl f(a.begin(), a.begin() + min(sz(a), 2 * k));
        vl t = ntt_mul(ntt_mul(b, b), f);
        b.resize(2 * k);
        for (int i = 0; i < 2 * k; i++)
            b[i] = ((2 * b[i] - (i < sz(t) ? t[i] : 0)) % MOD2 + MOD2) % MOD2;
    }
    b.resize(n);
    return b;
}

pair<vl, vl> poly_divmod(const vl &a, const vl &b) {
    int n = sz(a), m = sz(b);
    if (n < m) return {{}, a};
    vl ra(rall(a)), rb(rall(b));
    ra.resize(n - m + 1);
    vl q = ntt_mul(ra, poly_inv(rb, n - m + 1));
    q.resize(n - m + 1);
    reverse(all(q));
    vl r = ntt_mul(q, b);
    r.resize(m - 1);
    for (int i = 0; i < m - 1; i++) r[i] = (a[i] - r[i] + MOD2) % MOD2;
    return {q, r};
}

vl poly_log(const vl &a, int n) {
    vl d(max(0, sz(a) - 1));
    for (int i = 1; i < sz(a); i++) d[i - 1] = a[i] * i % MOD2;
    vl t = ntt_mul(d, poly_inv(a, n));
    vl r(n, 0), inv = inv_table(n, MOD2);
    for (int i = 1; i < n && i - 1 < sz(t); i++)
        r[i] = t[i - 1] * inv[i] % MOD2;
    return r;
}

vl poly_exp(const vl &a, int n) {
    vl b = {1};
    for (int k = 1; k < n; k *= 2) {
        vl l = poly_log(b, 2 * k);
        for (int i = 0; i < 2 * k; i++)
            l[i] = ((i < sz(a) ? a[i] : 0) - l[i] + MOD2) % MOD2;
        l[0] = (l[0] + 1) % MOD2;
        b = ntt_mul(b, l);
        b.resize(2 * k);
    }
    b.resize(n);
    return b;
}

vl poly_pow(const vl &a, ll k, int n) {
    vl r(n, 0);
    if (k == 0) { if (n) r[0] = 1; return r; }
    int t = 0;
    while (t < sz(a) && a[t] == 0) t++;
    if (t == sz(a) || (i128)t * k >= n) return r;
    int m = n - t * k;  // a = c x^t (1 + ...)
    ll c = a[t], ic = invmod_prime(c, MOD2), ck = powmod(c, k, MOD2);
    vl b(a.begin() + t, a.begin() + min(sz(a), t + m));
    for (ll &x : b) x = x * ic % MOD2;
    vl l = poly_log(b, m);
    for (ll &x : l) x = x * (k % MOD2) % MOD2;
    l = poly_exp(l, m);
    for (int i = 0; i < m; i++) r[i + t * k] = l[i] * ck % MOD2;
    return r;
}

vl poly_eval(const vl &a, const vl &xs) {
    int m = sz(xs);
    if (!m) return {};
    vector<vl> tr(4 * m);  // tr[v] = prod (x - xs[i]) over the node
    auto build = [&](auto &&self, int v, int l, int r) -> void {
        if (r - l == 1) { tr[v] = {(MOD2 - xs[l] % MOD2) % MOD2, 1}; return; }
        int md = (l + r) / 2;
        self(self, 2 * v, l, md), self(self, 2 * v + 1, md, r);
        tr[v] = ntt_mul(tr[2 * v], tr[2 * v + 1]);
    };
    build(build, 1, 0, m);
    vl res(m);
    auto go = [&](auto &&self, int v, int l, int r, vl p) -> void {
        p = poly_divmod(p, tr[v]).second;
        if (r - l == 1) { res[l] = p.empty() ? 0 : p[0]; return; }
        int md = (l + r) / 2;
        self(self, 2 * v, l, md, p), self(self, 2 * v + 1, md, r, p);
    };
    go(go, 1, 0, m, a);
    return res;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_poly() {
    vl iv = poly_inv({1, MOD2 - 1}, 4);  // 1/(1-x) = 1 + x + x^2 + ...
    cout << iv[0] << ' ' << iv[1] << ' ' << iv[2] << ' ' << iv[3]  // 1 1 1 1
         << '\n';
    auto [q, r] = poly_divmod({5, 0, 3, 1}, {1, 1});  // x^3+3x^2+5 by x+1
    cout << q[0] << ' ' << q[1] << ' ' << q[2]  // 998244351 2 1 r 7
         << " r " << r[0] << '\n';
    vl e = poly_exp({0, 1}, 5);  // e^x: k-th coef = 1/k!
    cout << e[2] << ' ' << e[2] * 2 % MOD2 << ' '  // 499122177 1 1
         << e[4] * 24 % MOD2 << '\n';
    vl p = poly_pow({0, 1, 1}, 3, 6);  // (x + x^2)^3 = x^3 + 3x^4 + 3x^5 + x^6
    cout << p[3] << ' ' << p[4] << ' ' << p[5] << '\n';  // 1 3 3
    vl v = poly_eval({1, 2, 3}, {0, 1, 2, 10});  // 1 + 2x + 3x^2
    cout << v[0] << ' ' << v[1] << ' ' << v[2] << ' ' << v[3]  // 1 6 17 321
         << '\n';
}
