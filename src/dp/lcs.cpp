/**
 * Usage: works on string or vector (any T with ==)
 *        lcs(a, b)                 -> one longest common subsequence
 *        lcs3(a, b, c)             -> LCS length of three sequences
 *        edit_distance(a, b, ins, del, rep) -> min cost to turn a into b
 *                                     (default costs 1; Levenshtein)
 * Complexity: O(|a| |b|) (lcs3 O(|a| |b| |c|)); memory the same
 * Source: UPB LCS (+ all LCS backtracking) + UMSS LCS / LCS 3 (recursive,
 *         fixed 100^3 globals) + UMSS Edit Distance + old lcs /
 *         edit_distance (CP4); weighted costs: own
 * Note: LCS of two permutations in O(n log n): lis.cpp on positions.
 *       Only the length with O(|b|) memory: keep two rows.
 * Status: untested
 */
template <class S>
S lcs(const S &a, const S &b) {
    int n = sz(a), m = sz(b);
    vector<vi> dp(n + 1, vi(m + 1, 0));
    for (int i = n - 1; i >= 0; i--)
        for (int j = m - 1; j >= 0; j--)
            dp[i][j] = a[i] == b[j] ? dp[i + 1][j + 1] + 1
                                    : max(dp[i + 1][j], dp[i][j + 1]);
    S r;
    for (int i = 0, j = 0; i < n && j < m;) {
        if (a[i] == b[j]) r.push_back(a[i]), i++, j++;
        else if (dp[i + 1][j] >= dp[i][j + 1]) i++;
        else j++;
    }
    return r;
}

template <class S>
int lcs3(const S &a, const S &b, const S &c) {
    int n = sz(a), m = sz(b), k = sz(c);
    vector<vector<vi>> dp(n + 1, vector<vi>(m + 1, vi(k + 1, 0)));
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++)
            for (int l = 1; l <= k; l++)
                dp[i][j][l] = a[i - 1] == b[j - 1] && b[j - 1] == c[l - 1]
                                  ? dp[i - 1][j - 1][l - 1] + 1
                                  : max({dp[i - 1][j][l], dp[i][j - 1][l],
                                         dp[i][j][l - 1]});
    return dp[n][m][k];
}

template <class S>
ll edit_distance(const S &a, const S &b, ll ins = 1, ll del = 1, ll rep = 1) {
    int n = sz(a), m = sz(b);
    vector<vl> dp(n + 1, vl(m + 1));
    for (int i = 0; i <= n; i++)
        for (int j = 0; j <= m; j++) {
            if (!i || !j) { dp[i][j] = i * del + j * ins; continue; }
            ll sub = dp[i - 1][j - 1] + (a[i - 1] == b[j - 1] ? 0 : rep);
            dp[i][j] = min({dp[i - 1][j] + del, dp[i][j - 1] + ins, sub});
        }
    return dp[n][m];
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_lcs() {
    cout << lcs(string("AGGTAB"), string("GXTXAYB")) << '\n';  // GTAB
    vi x = lcs(vi{1, 3, 4, 1}, vi{3, 4, 1, 2, 1});
    cout << sz(x) << ' ' << x[0] << x[1] << x[2] << '\n';      // 3 341
    string p = "AGGT12", q = "12TXAYB", r = "12XBA";
    cout << lcs3(p, q, r) << '\n';                              // 2
    cout << edit_distance(string("kitten"), string("sitting")) << '\n';  // 3
}
