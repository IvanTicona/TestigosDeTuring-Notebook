/**
 * Usage: subsetSum(n, X, a) -> true si algun subconjunto de a[0..n-1] suma X
 * Complexity: O(n * X) tiempo y memoria
 * Status: untested
 */
bool subsetSum(int n, int X, const vector<int> &a) {
    // dp[i][j] = true si con los primeros i elementos se puede formar suma j
    vector<vector<bool>> dp(n+1, vector<bool>(X+1, false));
    dp[0][0] = true; // con 0 elementos solo puedo formar suma 0

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= X; j++) {
            dp[i][j] = dp[i-1][j]; // no uso a[i-1]
            if (j >= a[i-1]) {
                dp[i][j] = dp[i][j] || dp[i-1][j - a[i-1]]; // uso a[i-1]
            }
        }
    }

    return dp[n][X];
}
