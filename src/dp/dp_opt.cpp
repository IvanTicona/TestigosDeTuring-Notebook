/**
 * Usage: partition_dnc(n, k, C) -> min cost to split [0, n) into k
 *          non-empty consecutive groups, C(j, i) = cost of [j, i)
 *        dnc_layer(prev, cur, C)  -> one layer: cur[i] = min_{j<i}
 *          prev[j] + C(j, i) (building block for other recurrences)
 *        knuth(n, w) -> dp[0][n-1], dp[i][j] = min_{i<=m<j} dp[i][m] +
 *          dp[m+1][j] + w(i, j), dp[i][i] = 0 (merge / split intervals)
 *        aliens(k, lo, hi, f) -> g(k) for convex g from f(lambda) =
 *          {min_c g(c) + lambda c, SMALLEST optimal c}, lambda in [lo, hi]
 * Complexity: dnc O(k n log n) evaluations of C; knuth O(n^2);
 *             aliens O(log(hi - lo)) calls of f
 * Source: UPB DP_Optimizations (cheat sheet + D&C template) + UMSS
 *         Knuth's Optimization (D&C was a TODO); aliens: own
 * Note: D&C needs opt(i) <= opt(i+1); Knuth needs opt[i][j-1] <= opt[i][j]
 *       <= opt[i+1][j]. Both hold if C / w satisfies the quadrangle
 *       inequality w(a,c) + w(b,d) <= w(a,d) + w(b,c), a <= b <= c <= d
 *       (and w monotone on inclusion for Knuth). Aliens: "exactly k
 *       items" -> pay lambda per item, binary search lambda; ties MUST
 *       return the fewest items. hi = max |slope| of g (e.g. max cost).
 * Status: untested
 */
template <class F>
void dnc_rec(int l, int r, int ol, int orr, const vl &prev, vl &cur, F &C) {
    if (l > r) return;
    int m = (l + r) / 2, best = ol;
    ll bv = INFL;
    for (int j = ol; j <= min(m - 1, orr); j++)
        if (prev[j] < INFL) {
            ll v = prev[j] + C(j, m);
            if (v < bv) bv = v, best = j;
        }
    cur[m] = bv;
    dnc_rec(l, m - 1, ol, best, prev, cur, C);
    dnc_rec(m + 1, r, best, orr, prev, cur, C);
}
template <class F>
void dnc_layer(const vl &prev, vl &cur, F C) {
    cur.assign(sz(prev), INFL);
    dnc_rec(0, sz(prev) - 1, 0, sz(prev) - 1, prev, cur, C);
}

template <class F>
ll partition_dnc(int n, int k, F C) {
    vl dp(n + 1, INFL), nx;
    dp[0] = 0;
    for (int t = 0; t < k; t++) dnc_layer(dp, nx, C), swap(dp, nx);
    return dp[n];
}

template <class W>
ll knuth(int n, W w) {
    vector<vl> dp(n, vl(n, 0));
    vector<vi> opt(n, vi(n));
    for (int i = 0; i < n; i++) opt[i][i] = i;
    for (int len = 2; len <= n; len++)
        for (int i = 0; i + len - 1 < n; i++) {
            int j = i + len - 1;
            dp[i][j] = INFL;
            ll c = w(i, j);
            for (int m = opt[i][j - 1]; m <= min(j - 1, opt[i + 1][j]); m++)
                if (dp[i][m] + dp[m + 1][j] + c < dp[i][j])
                    dp[i][j] = dp[i][m] + dp[m + 1][j] + c, opt[i][j] = m;
        }
    return dp[0][n - 1];
}

template <class F>
ll aliens(ll k, ll lo, ll hi, F f) {  // smallest lambda with count <= k
    while (lo < hi) {
        ll md = lo + (hi - lo) / 2;
        if (f(md).S <= k) hi = md;
        else lo = md + 1;
    }
    return f(lo).F - lo * k;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_dp_opt() {
    vl a = {1, 3, 2, 4, 1}, pre(6, 0);
    for (int i = 0; i < 5; i++) pre[i + 1] = pre[i] + a[i];
    auto C = [&](int j, int i) { return SQ(pre[i] - pre[j]); };  // sum^2
    // best split [1 3 2][4 1]: 36 + 25
    cout << partition_dnc(5, 2, C) << '\n';  // 61
    // merging adjacent piles 1 3 2 4, cost = size of the merged pile
    auto w = [&](int i, int j) { return pre[j + 1] - pre[i]; };
    cout << knuth(4, w) << '\n';             // 20
    // aliens: exactly 2 groups via a penalty per group (O(n^2) inner dp)
    auto f = [&](ll lam) {
        vector<pll> d(6, {INFL, 0});
        d[0] = {0, 0};
        for (int i = 1; i <= 5; i++)
            for (int j = 0; j < i; j++)
                d[i] = min(d[i], pll{d[j].F + C(j, i) + lam, d[j].S + 1});
        return d[5];
    };
    cout << aliens(2, 0, 200, f) << '\n';   // 61
}
