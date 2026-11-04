/**
 * Usage: w = weights, v = values, W = capacity; all return max value with
 *        total weight <= W
 *        knap01(w, v, W)           each item at most once
 *        knap01_items(w, v, W)     -> indices of an optimal 0/1 choice
 *        knap_unbounded(w, v, W)   unlimited copies
 *        knap_bounded(w, v, c, W)  item i at most c[i] times
 *        knap_by_value(w, v, W)    0/1 when W is huge (1e9) but sum v small
 * Complexity: O(n W); bounded O(W sum log c_i); by_value O(n sum v)
 * Source: UPB Knapsack01 + old knapsack_01 (CP4) + UMSS Multiple Knapsack
 *         (binary grouping; value and weight vectors swapped by name);
 *         items, by_value: own
 * Note: exactly W instead of <= W: init dp = -INFL except dp[0] = 0.
 *       Weights / values >= 0. Count ways / subset sums: subset_sum.cpp.
 * Status: untested
 */
ll knap01(const vl &w, const vl &v, int W) {
    vl dp(W + 1, 0);
    for (int i = 0; i < sz(w); i++)
        for (int c = W; c >= w[i]; c--) dp[c] = max(dp[c], dp[c - w[i]] + v[i]);
    return dp[W];
}

vi knap01_items(const vl &w, const vl &v, int W) {
    int n = sz(w);
    vl dp(W + 1, 0);
    vector<vector<bool>> take(n, vector<bool>(W + 1, false));
    for (int i = 0; i < n; i++)
        for (int c = W; c >= w[i]; c--)
            if (dp[c - w[i]] + v[i] > dp[c])
                dp[c] = dp[c - w[i]] + v[i], take[i][c] = true;
    vi r;
    for (int i = n - 1, c = W; i >= 0; i--)
        if (take[i][c]) r.pb(i), c -= w[i];
    reverse(all(r));
    return r;
}

ll knap_unbounded(const vl &w, const vl &v, int W) {
    vl dp(W + 1, 0);
    for (int i = 0; i < sz(w); i++)
        for (int c = w[i]; c <= W; c++) dp[c] = max(dp[c], dp[c - w[i]] + v[i]);
    return dp[W];
}

ll knap_bounded(const vl &w, const vl &v, const vl &cnt, int W) {
    vl nw, nv;  // split c copies into 1, 2, 4, ..., rest (binary grouping)
    for (int i = 0; i < sz(w); i++)
        for (ll k = cnt[i], p = 1; k > 0; k -= p, p *= 2) {
            ll t = min(p, k);
            nw.pb(w[i] * t), nv.pb(v[i] * t);
        }
    vl dp(W + 1, 0);
    for (int i = 0; i < sz(nw); i++)
        for (ll c = W; c >= nw[i]; c--)
            dp[c] = max(dp[c], dp[c - nw[i]] + nv[i]);
    return dp[W];
}

ll knap_by_value(const vl &w, const vl &v, ll W) {
    ll V = accumulate(all(v), 0LL);
    vl mw(V + 1, INFL);  // mw[x] = min weight reaching value exactly x
    mw[0] = 0;
    for (int i = 0; i < sz(w); i++)
        for (ll x = V; x >= v[i]; x--)
            if (mw[x - v[i]] < INFL) mw[x] = min(mw[x], mw[x - v[i]] + w[i]);
    for (ll x = V;; x--) if (mw[x] <= W) return x;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_knapsack() {
    vl w = {3, 2, 1}, v = {5, 3, 4};
    cout << knap01(w, v, 5) << ' ' << knap_unbounded(w, v, 5) << '\n';  // 9 20
    vi it = knap01_items(w, v, 5);  // items 0 and 2: weight 4, value 9
    cout << sz(it) << ' ' << it[0] << ' ' << it[1] << '\n';  // 2 0 2
    cout << knap_bounded(w, v, {1, 1, 2}, 5) << '\n';         // 13
    cout << knap_by_value({1000000000, 1}, {7, 3}, 1000000000) << '\n';  // 7
}
