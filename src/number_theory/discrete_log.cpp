/**
 * Usage: discrete_log(a, b, m) -> smallest x >= 0 with a^x == b (mod m),
 *                                 or -1. Any m >= 1, gcd(a, m) may be > 1.
 *        discrete_root(k, a, p)   -> some x with x^k == a (mod p), or -1
 * Complexity: O(sqrt(m)) time and memory (hash table)
 * Source: UMSS Discrete Log (= cp-algorithms, handles gcd != 1);
 *         fixed m = 1 (returned 1) and overflow for m > 3e9. UPB BSGS is
 *         BROKEN (answers when no solution exists). discrete_root: own.
 * Note: discrete_root needs p prime. If x0 is a root, all roots are
 *       x0 * g^(i * (p-1) / gcd(k, p-1)), g = primitive_root(p).
 * Depends: src/number_theory/modular_arithmetic.cpp
 * Depends: src/number_theory/primitive_root.cpp
 * Status: ok
 */
ll discrete_log(ll a, ll b, ll m) {
    if (m == 1) return 0;
    a %= m; if (a < 0) a += m;
    b %= m; if (b < 0) b += m;
    ll k = 1, add = 0, g;  // invariant: answer = add + x with k*a^x == b
    while ((g = gcd(a, m)) > 1) {
        if (b == k) return add;
        if (b % g) return -1;
        b /= g, m /= g, add++;
        k = (ll)((__int128)k * a / g % m);
    }
    ll n = (ll)sqrtl((long double)m) + 1, an = powmod(a, n, m);
    unordered_map<ll, ll, custom_hash> vals;  // b * a^q -> largest q
    vals.reserve(2 * n);
    for (ll q = 0, cur = b; q <= n; q++, cur = mulmod(cur, a, m)) vals[cur] = q;
    for (ll p = 1, cur = k; p <= n; p++) {
        cur = mulmod(cur, an, m);  // k * a^(n*p)
        auto it = vals.find(cur);
        if (it != vals.end()) return n * p - it->S + add;
    }
    return -1;
}

ll discrete_root(ll k, ll a, ll p) {
    a %= p; if (a < 0) a += p;
    if (a == 0) return 0;
    ll g = primitive_root(p);
    ll y = discrete_log(powmod(g, k, p), a, p);  // (g^k)^y == a
    return y == -1 ? -1 : (ll)powmod(g, y, p);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_discrete_log() {
    cout << discrete_log(2, 3, 13) << '\n';         // 4
    cout << discrete_log(2, 1, 7) << '\n';          // 0
    cout << discrete_log(3, 0, 9) << '\n';          // 2
    cout << discrete_log(2, 3, 7) << '\n';          // -1
    ll r = discrete_root(3, 8, 13);                 // roots: 2, 5, 6
    cout << powmod(r, 3, 13) << '\n';               // 8
    cout << discrete_root(2, 3, 7) << '\n';         // -1
}
