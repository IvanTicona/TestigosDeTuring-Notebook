/**
 * Usage: catalan(cb, n)             -> C_n mod p (Comb cb(N >= 2n))
 *        catalan_any(N, m)          -> C_0..C_N mod ANY m, O(N^2)
 *        paths_nonneg(cb, L, x, y)  -> #paths of L steps +-1 from height x to
 *                                     height y never below 0 (reflection)
 *        brackets_with_prefix(cb, n, s) -> #valid bracket strings of length
 *                                         n that start with s
 *        narayana(cb, n, k)         -> #Dyck paths of 2n steps with k peaks
 * Complexity: O(1) each with Comb (O(|s|) for the prefix); catalan_any O(N^2)
 * Source: UMSS Catalan + Catalan Convolution (out-of-bounds fact[mxN]; own
 *         factorial copies) rewritten on Comb; generalized via reflection
 * Note: C_n = C(2n, n) / (n+1) = C(2n, n) - C(2n, n+1);
 *       C_{n+1} = sum C_i C_{n-i}. 1 1 2 5 14 42 132 429 1430 4862.
 *       Counts: Dyck paths, bracket strings, binary trees with n nodes,
 *       triangulations of an (n+2)-gon, non-crossing partitions.
 *       Reflection: paths x -> y never below 0 = C(L, u) - C(L, u + x + 1),
 *       u = (L + y - x) / 2 up-steps.
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/combinatorics/binomial.cpp
 * Status: ok
 */
ll catalan(const Comb &cb, ll n) {
    if (n < 0) return 0;
    return (cb.C(2 * n, n) - cb.C(2 * n, n + 1) + cb.p) % cb.p;
}

vl catalan_any(int N, ll m) {
    vl c(N + 1, 0);
    c[0] = 1 % m;
    for (int n = 0; n < N; n++)
        for (int i = 0; i <= n; i++)
            c[n + 1] = (c[n + 1] + (ll)mulmod(c[i], c[n - i], m)) % m;
    return c;
}

ll paths_nonneg(const Comb &cb, ll L, ll x, ll y) {
    if (x < 0 || y < 0 || L < 0 || (L + y - x) % 2 != 0) return 0;
    ll u = (L + y - x) / 2;  // number of +1 steps
    if (u < 0 || u > L) return 0;
    return (cb.C(L, u) - cb.C(L, u + x + 1) + cb.p) % cb.p;
}

ll brackets_with_prefix(const Comb &cb, ll n, const string &s) {
    ll h = 0;
    for (char ch : s) {
        h += ch == '(' ? 1 : -1;
        if (h < 0) return 0;
    }
    if ((ll)sz(s) > n) return 0;
    return paths_nonneg(cb, n - sz(s), h, 0);
}

ll narayana(const Comb &cb, ll n, ll k) {  // C(n,k) C(n,k-1) / n
    if (n <= 0 || k < 1 || k > n) return 0;
    return cb.C(n, k) * cb.C(n, k - 1) % cb.p * cb.inv(n) % cb.p;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_catalan() {
    Comb cb(200000);
    string s;
    for (int n = 0; n < 10; n++) s += to_string(catalan(cb, n)) + " ";
    cout << s << '\n';                    // 1 1 2 5 14 42 132 429 1430 4862
    cout << catalan(cb, 100000) << '\n';  // 945729344
    cout << catalan_any(9, 1000)[9] << '\n';  // 862
    cout << brackets_with_prefix(cb, 6, "(()") << '\n';  // 2
    cout << paths_nonneg(cb, 10, 2, 0) << '\n';  // 90
    cout << narayana(cb, 4, 2) << '\n';   // 6
}
