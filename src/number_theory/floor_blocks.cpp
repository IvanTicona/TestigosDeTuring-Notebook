/**
 * Usage: for_floor_blocks(n, [&](ll l, ll r, ll q) { ... });
 *          -> every maximal [l, r] (1 <= l <= r <= n) with n / i == q
 *        for_floor_blocks2(n, m, f(l, r, qn, qm)) -> n / i and m / i both
 *          constant on [l, r], for 1 <= i <= min(n, m)
 *        sum_floor(n) = sum_{i<=n} n/i = sum_{k<=n} d(k)
 *        sum_sigma(n) = sum_{k<=n} sigma(k);  sum_mod(n) = sum_{i<=n} n % i
 * Complexity: O(sqrt n) blocks (2 sqrt n at most)
 * Source: UMSS SQRT Mod (idea only, fragment) + UTP cumulative sum of
 *         divisors; code own
 * Note: n / i takes < 2 sqrt(n) distinct values; inside a block,
 *       r = n / (n / l). Ceil: ceil(n / i) = (n - 1) / i + 1 (blocks of n-1).
 *       sum_sigma and sum_mod overflow ll for n > ~3e9 (use __int128).
 * Status: ok
 */
template <class F> void for_floor_blocks(ll n, F f) {
    for (ll l = 1, r; l <= n; l = r + 1) {
        ll q = n / l;
        r = n / q;
        f(l, r, q);
    }
}

template <class F> void for_floor_blocks2(ll n, ll m, F f) {
    for (ll l = 1, r; l <= min(n, m); l = r + 1) {
        r = min(n / (n / l), m / (m / l));
        f(l, r, n / l, m / l);
    }
}

ll sum_floor(ll n) {  // sum_{i=1}^{n} floor(n / i)
    ll s = 0;
    for_floor_blocks(n, [&](ll l, ll r, ll q) { s += q * (r - l + 1); });
    return s;
}

ll sum_sigma(ll n) {  // sum_{i=1}^{n} i * floor(n / i)
    ll s = 0;
    for_floor_blocks(n, [&](ll l, ll r, ll q) {
        s += q * ((l + r) * (r - l + 1) / 2);
    });
    return s;
}

ll sum_mod(ll n) { return n * n - sum_sigma(n); }  // n % i = n - i*(n/i)

// ---- Example: call from solve(); each // is the line it prints ----
void example_floor_blocks() {
    string s;
    for_floor_blocks(10, [&](ll l, ll r, ll q) {
        s += "[" + to_string(l) + "," + to_string(r) + "]=";
        s += to_string(q) + " ";
    });
    cout << s << '\n';  // [1,1]=10 [2,2]=5 [3,3]=3 [4,5]=2 [6,10]=1
    cout << sum_floor(10) << ' ' << sum_sigma(10) << '\n';  // 27 87
    cout << sum_mod(10) << '\n';          // 13
    cout << sum_floor(1e12) << '\n';      // 27785452449086
}
