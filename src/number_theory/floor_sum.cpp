/**
 * Usage: floor_sum(n, m, a, b) = sum_{i=0}^{n-1} floor((a*i + b) / m)
 *        mod_sum(n, m, a, b)   = sum_{i=0}^{n-1} (a*i + b) mod m
 * Complexity: O(log m) (Euclid-like)
 * Source: UMSS Floor Sums = AtCoder Library floor_sum (unchanged logic);
 *         mod_sum: own, from (x mod m) = x - m*floor(x/m)
 * Note: 0 <= n < 2^32, 1 <= m < 2^32, any a, b (negatives ok). The result
 *       is exact if it fits in ll, otherwise it is correct modulo 2^64.
 * Status: ok
 */
ull floor_sum_unsigned(ull n, ull m, ull a, ull b) {
    ull ans = 0;
    while (true) {
        if (a >= m) ans += n * (n - 1) / 2 * (a / m), a %= m;
        if (b >= m) ans += n * (b / m), b %= m;
        ull y_max = a * n + b;
        if (y_max < m) break;
        n = y_max / m, b = y_max % m;  // count lattice points the other way
        swap(m, a);
    }
    return ans;
}

ll floor_sum(ll n, ll m, ll a, ll b) {
    ull ans = 0;
    if (a < 0) {
        ll a2 = (a % m + m) % m;
        ans -= 1ULL * n * (n - 1) / 2 * ((a2 - a) / m);
        a = a2;
    }
    if (b < 0) {
        ll b2 = (b % m + m) % m;
        ans -= 1ULL * n * ((b2 - b) / m);
        b = b2;
    }
    return ans + floor_sum_unsigned(n, m, a, b);
}

ll mod_sum(ll n, ll m, ll a, ll b) {  // needs the true sum of a*i+b in ll
    ll lin = a * (n * (n - 1) / 2) + b * n;
    return lin - m * floor_sum(n, m, a, b);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_floor_sum() {
    // floor(i/2) for i = 0..4: 0 0 1 1 2
    cout << floor_sum(5, 2, 1, 0) << '\n';          // 4
    cout << floor_sum(4, 10, 6, 3) << '\n';         // 3
    cout << floor_sum(3, 5, -7, 1) << '\n';         // -5
    cout << mod_sum(4, 10, 6, 3) << '\n';           // 18
}
