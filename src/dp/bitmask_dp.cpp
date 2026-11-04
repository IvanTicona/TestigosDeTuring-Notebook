/**
 * Usage: tsp(d)          -> {min cost cycle through all nodes, order from 0}
 *                           (-1 if none; d[i][j] = INFL: no edge)
 *        ham_path(d)     -> {min cost path visiting all nodes, order}
 *        sos(f, sub, inv)-> in place: sub ? F[m] = sum f[s], s subset of m
 *                           : sum over supersets; inv = undo (Mobius)
 *        partition_dp(c) -> min over partitions of the full set into
 *                           groups g of sum c[g] (c indexed by mask)
 *        min_rides(w, X) -> min groups of total weight <= X (w[i] <= X)
 * Complexity: tsp / ham_path O(2^n n^2); sos O(2^n n);
 *             partition_dp O(3^n); min_rides O(2^n n)
 * Source: UPB BitmaskDP_Templates + UPB TSP_HeldKarp_Reconstruct + UMSS
 *         TSP Cycle / Path (counting), DP Mask Over Submasks, SOS DP (+
 *         reverse), Elevator Problem
 * Note: submasks of m: for (int s = m; s; s = (s - 1) & m) (add s = 0
 *       apart). All masks with their submasks = 3^n. SOS with max / min or
 *       mod p: change the += (fwht.cpp has the mod-p versions and subset
 *       convolution). Counting Hamiltonian paths: same DP with + instead
 *       of min.
 * Status: untested
 */
pair<ll, vi> held_karp(const vector<vl> &d, bool cycle) {
    int n = sz(d), N = 1 << n;
    vector<vl> dp(N, vl(n, INFL));
    vector<vi> par(N, vi(n, -1));
    for (int i = 0; i < (cycle ? 1 : n); i++) dp[1 << i][i] = 0;
    for (int m = 1; m < N; m++)
        for (int i = 0; i < n; i++)
            if (dp[m][i] < INFL)
                for (int j = 0; j < n; j++)
                    if (!(m >> j & 1) && d[i][j] < INFL) {
                        ll c = dp[m][i] + d[i][j];
                        int nm = m | 1 << j;
                        if (c < dp[nm][j]) dp[nm][j] = c, par[nm][j] = i;
                    }
    ll best = INFL;
    int last = -1;
    for (int i = 0; i < n; i++) {
        if (cycle && n > 1 && (i == 0 || d[i][0] >= INFL)) continue;
        ll c = dp[N - 1][i] + (cycle && n > 1 ? d[i][0] : 0);
        if (dp[N - 1][i] < INFL && c < best) best = c, last = i;
    }
    if (last < 0) return {-1, {}};
    vi path;
    for (int m = N - 1, v = last; v != -1;) {
        path.pb(v);
        int p = par[m][v];
        m ^= 1 << v, v = p;
    }
    reverse(all(path));
    return {best, path};
}
pair<ll, vi> tsp(const vector<vl> &d) { return held_karp(d, true); }
pair<ll, vi> ham_path(const vector<vl> &d) { return held_karp(d, false); }

void sos(vl &f, bool sub = true, bool inv = false) {
    int N = sz(f);
    for (int b = 1; b < N; b <<= 1)
        for (int m = 0; m < N; m++)
            if (!(m & b) == !sub) f[m] += (inv ? -1 : 1) * f[m ^ b];
}

ll partition_dp(const vl &c) {  // c[0] unused
    int N = sz(c);
    vl dp(N, INFL);
    dp[0] = 0;
    for (int m = 1; m < N; m++)
        for (int s = m; s; s = (s - 1) & m)
            if ((s & -s) == (m & -m) && dp[m ^ s] < INFL)  // s holds m's
                dp[m] = min(dp[m], dp[m ^ s] + c[s]);       // lowest bit
    return dp[N - 1];
}

int min_rides(const vl &w, ll X) {
    int n = sz(w), N = 1 << n;
    if (!n) return 0;
    vector<pll> dp(N, {n + 1, 0});  // {rides, load of the last ride}
    dp[0] = {1, 0};
    for (int m = 1; m < N; m++)
        for (int i = 0; i < n; i++)
            if (m >> i & 1) {
                auto [r, l] = dp[m ^ 1 << i];
                if (l + w[i] <= X) l += w[i];
                else r++, l = w[i];
                dp[m] = min(dp[m], pll{r, l});
            }
    return dp[N - 1].F;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_bitmask_dp() {
    vector<vl> d = {{0, 10, 15, 20}, {10, 0, 35, 25},
                    {15, 35, 0, 30}, {20, 25, 30, 0}};
    auto [c, p] = tsp(d);  // 0 -> 2 -> 3 -> 1 -> 0
    cout << c << ' ' << p[0] << p[1] << p[2] << p[3] << '\n';  // 80 0231
    // best path 2 -> 0 -> 1 -> 3 (no return edge)
    cout << ham_path(d).F << '\n';                             // 50
    vl f = {1, 2, 3, 4};  // sums over submasks: 1, 1+2, 1+3, 1+2+3+4
    sos(f);
    cout << f[0] << ' ' << f[1] << ' ' << f[2] << ' ' << f[3]  // 1 3 4 10
         << '\n';
    cout << min_rides({2, 3, 3, 5, 6}, 10) << '\n';  // 2
}
