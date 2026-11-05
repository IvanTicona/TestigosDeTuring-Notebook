/**
 * Usage: (all tables mod ANY m >= 1, index 0..N)
 *        derangements(N, m)[n]  -> permutations of n with no fixed point
 *        stirling2(N, m)[n][k]  -> partitions of n labeled items into k sets
 *        stirling1(N, m)[n][k]  -> permutations of n with k cycles (unsigned)
 *        bell(N, m)[n]          -> partitions of an n-set (sum_k S2(n, k))
 *        partitions(N, m)[n]    -> p(n): ways to write n as unordered sum
 *        stirling2_single(cb, n, k) -> S2(n, k) mod p, n huge, O(k log n)
 * Complexity: derangements O(N); stirling tables O(N^2); bell O(N^2);
 *             partitions O(N sqrt N) (Euler pentagonal theorem)
 * Source: UMSS SubFactorial (d_n = (n-1)(d_{n-1} + d_{n-2})); UPB isc
 *         partition (pentagonal; was Java syntax). Stirling, Bell: own
 * Note: D: 1 0 1 2 9 44 265, D_n = n! sum (-1)^i / i!.
 *       S2(n,k) = k S2(n-1,k) + S2(n-1,k-1);  s1(n,k) = (n-1) s1(n-1,k) +
 *       s1(n-1,k-1). Bell: 1 1 2 5 15 52 203 877. p(n): 1 1 2 3 5 7 11 15.
 *       S2(n,k) = (1/k!) sum_j (-1)^j C(k,j) (k-j)^n.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/combinatorics/binomial.cpp
 * Status: ok
 */
vl derangements(int N, ll m) {
    vl d(N + 1, 0);
    d[0] = 1 % m;
    for (int n = 2; n <= N; n++)
        d[n] = (ll)mulmod(n - 1, (d[n - 1] + d[n - 2]) % m, m);
    return d;
}

vector<vl> stirling2(int N, ll m) {
    vector<vl> S(N + 1, vl(N + 1, 0));
    S[0][0] = 1 % m;
    for (int n = 1; n <= N; n++)
        for (int k = 1; k <= n; k++)
            S[n][k] = ((ll)mulmod(k, S[n - 1][k], m) + S[n - 1][k - 1]) % m;
    return S;
}

vector<vl> stirling1(int N, ll m) {
    vector<vl> s(N + 1, vl(N + 1, 0));
    s[0][0] = 1 % m;
    for (int n = 1; n <= N; n++)
        for (int k = 1; k <= n; k++)
            s[n][k] = ((ll)mulmod(n - 1, s[n - 1][k], m) + s[n - 1][k - 1]) % m;
    return s;
}

vl bell(int N, ll m) {  // Bell triangle: each row starts with the last value
    vl b(N + 1, 0), row = {1 % m};
    b[0] = 1 % m;
    for (int n = 1; n <= N; n++) {
        vl nr = {row.back()};
        for (ll x : row) nr.pb((nr.back() + x) % m);
        b[n] = nr[0];
        row = nr;
    }
    return b;
}

vl partitions(int N, ll m) {  // p(n) = sum_k (-1)^(k+1) p(n - k(3k-1)/2) ...
    vl p(N + 1, 0);
    p[0] = 1 % m;
    for (int n = 1; n <= N; n++)
        for (int k = 1;; k++) {
            ll g1 = (ll)k * (3 * k - 1) / 2, g2 = (ll)k * (3 * k + 1) / 2;
            if (g1 > n) break;
            ll add = (p[n - g1] + (g2 <= n ? p[n - g2] : 0)) % m;  // < 2m
            p[n] = (k & 1) ? (p[n] + add) % m : (p[n] - add + m) % m;
        }
    return p;
}

ll stirling2_single(const Comb &cb, ll n, ll k) {  // k <= N of cb
    if (k < 0 || n < 0) return 0;
    if (n == 0) return k == 0;
    ll s = 0;
    for (ll j = 0; j <= k; j++) {
        ll t = cb.C(k, j) * powmod(k - j, n, cb.p) % cb.p;
        s = (j & 1) ? (s - t + cb.p) % cb.p : (s + t) % cb.p;
    }
    return s * cb.fi[k] % cb.p;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_counting_sequences() {
    vl d = derangements(20, (ll)4e18);
    cout << d[4] << ' ' << d[20] << '\n';            // 9 895014631192902121
    auto S = stirling2(10, MOD), s = stirling1(10, MOD);
    cout << S[5][2] << ' ' << S[10][3] << '\n';      // 15 9330
    cout << s[5][2] << ' ' << s[10][3] << '\n';      // 50 1172700
    cout << bell(20, (ll)4e18)[20] << '\n';          // 51724158235372
    vl p = partitions(200, (ll)4e18);
    cout << p[100] << ' ' << p[200] << '\n';         // 190569292 3972999029388
    Comb cb(100);
    cout << stirling2_single(cb, 1000, 30) << '\n';  // 552283983
}
