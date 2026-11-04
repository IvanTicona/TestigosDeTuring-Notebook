/**
 * Usage: lis(a, strict = true) -> indices of one longest strictly
 *                                 increasing (strict = false: non-
 *                                 decreasing) subsequence
 *        count_lis(a, m)       -> {length, #strictly increasing
 *                                 subsequences of that length mod m}
 * Complexity: O(n log n)
 * Source: UMSS LIS (length only) + UPB LongestIncreasingSubsequence
 *         (reconstruction, pair trick); count_lis: own (Fenwick of
 *         (len, count))
 * Note: decreasing -> negate values. LCS of two permutations = LIS of
 *       pos_in_b[a[i]]. Min #non-increasing subsequences covering a
 *       (Dilworth) = LIS strict length. Box nesting: sort by (w asc, h desc),
 *       then strict LIS on h.
 * Status: untested
 */
vi lis(const vl &a, bool strict = true) {
    int n = sz(a);
    vl tail;  // tail[k] = smallest last value of a length k+1 subsequence
    vi tid, par(n, -1);
    for (int i = 0; i < n; i++) {
        int k = (strict ? lower_bound(all(tail), a[i])
                        : upper_bound(all(tail), a[i])) - tail.begin();
        if (k == sz(tail)) tail.pb(a[i]), tid.pb(i);
        else tail[k] = a[i], tid[k] = i;
        par[i] = k ? tid[k - 1] : -1;
    }
    vi r;
    for (int i = tid.empty() ? -1 : tid.back(); i != -1; i = par[i]) r.pb(i);
    reverse(all(r));
    return r;
}

pair<int, ll> count_lis(const vl &a, ll m = MOD) {
    vl v = a;
    sort(all(v)), v.erase(unique(all(v)), v.end());
    int k = sz(v);
    vector<pair<int, ll>> bit(k + 1, {0, 0});
    auto join = [&](pair<int, ll> &x, pair<int, ll> y) {
        if (y.F > x.F) x = y;
        else if (y.F == x.F) x.S = (x.S + y.S) % m;
    };
    pair<int, ll> best = {0, 0};
    for (ll x : a) {
        int p = lower_bound(all(v), x) - v.begin();
        pair<int, ll> q = {0, 1 % m};  // best ending below x (empty: 1)
        for (int i = p; i > 0; i -= i & -i) join(q, bit[i]);
        q.F++;
        for (int i = p + 1; i <= k; i += i & -i) join(bit[i], q);
        join(best, q);
    }
    return best;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_lis() {
    vl a = {3, 1, 2, 2, 5, 4, 6};
    vi s = lis(a), t = lis(a, false);
    cout << sz(s) << ": " << a[s[0]] << ' ' << a[s[1]] << ' '  // 4: 1 2 4 6
         << a[s[2]] << ' ' << a[s[3]] << '\n';
    cout << sz(t) << '\n';                        // 5
    auto [len, cnt] = count_lis(a);  // 1 2 5 6, 1 2 4 6 (two 2's -> x2)
    cout << len << ' ' << cnt << '\n';            // 4 4
}
