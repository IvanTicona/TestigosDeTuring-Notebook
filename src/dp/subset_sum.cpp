/**
 * Usage: subset_sums<N>(a)       -> bitset: bit s set iff some subset of a
 *                                   sums to s (s < N); equal values grouped
 *        count_subsets(a, S, m)  -> #subsets (by index) with sum S, mod m
 *        coin_ways(c, X, m)      -> #multisets of coins summing to X
 *        coin_seqs(c, X, m)      -> #ordered sequences of coins summing to X
 *        coin_min(c, X)          -> fewest coins summing to X, -1 if none
 * Complexity: subset_sums O(N/64 * sum over values of log(count));
 *             with total sum S that is O(S sqrt S / 64); others O(n X)
 * Source: UPB SubsetSum + UPB CoinChange + UMSS Coin Change + old
 *         subset_sum (CP4); bitset with grouping of equal values: own
 * Note: coin_ways loops coins outside (each multiset once); coin_seqs
 *       loops sums outside (orders count). Values >= 0 (coins >= 1).
 *       Partition into 2 equal halves: subset_sums has bit total / 2.
 * Status: untested
 */
template <int N>
bitset<N> subset_sums(const vl &a) {
    map<ll, ll> cnt;  // k copies of x -> items x*1, x*2, x*4, ..., rest
    for (ll x : a) cnt[x]++;
    bitset<N> b;
    b[0] = 1;
    for (auto [x, k] : cnt)
        for (ll p = 1; k > 0; k -= p, p *= 2) {
            ll t = min(p, k);
            if (x * t < N) b |= b << (x * t);
        }
    return b;
}

ll count_subsets(const vl &a, int S, ll m = MOD) {
    vl dp(S + 1, 0);
    dp[0] = 1 % m;
    for (ll x : a)
        for (int s = S; s >= x; s--) dp[s] = (dp[s] + dp[s - x]) % m;
    return dp[S];
}

ll coin_ways(const vl &c, int X, ll m = MOD) {
    vl dp(X + 1, 0);
    dp[0] = 1 % m;
    for (ll x : c)
        for (int s = x; s <= X; s++) dp[s] = (dp[s] + dp[s - x]) % m;
    return dp[X];
}

ll coin_seqs(const vl &c, int X, ll m = MOD) {
    vl dp(X + 1, 0);
    dp[0] = 1 % m;
    for (int s = 1; s <= X; s++)
        for (ll x : c) if (x <= s) dp[s] = (dp[s] + dp[s - x]) % m;
    return dp[X];
}

ll coin_min(const vl &c, int X) {
    vl dp(X + 1, INFL);
    dp[0] = 0;
    for (int s = 1; s <= X; s++)
        for (ll x : c)
            if (x <= s && dp[s - x] < INFL) dp[s] = min(dp[s], dp[s - x] + 1);
    return dp[X] < INFL ? dp[X] : -1;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_subset_sum() {
    auto b = subset_sums<64>({3, 34, 4, 12, 5, 2});
    ll c6 = count_subsets({1, 2, 3, 3}, 6);  // {1,2,3}, {1,2,3'}, {3,3'}
    cout << b[9] << b[30] << ' ' << c6 << '\n';  // 10 3
    vl c = {1, 2, 5};
    cout << coin_ways(c, 5) << ' ' << coin_seqs(c, 5) << '\n';  // 4 9
    cout << coin_min(c, 11) << ' ' << coin_min({4, 6}, 7) << '\n';  // 3 -1
}
