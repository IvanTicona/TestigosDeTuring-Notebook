/**
 * Usage: subsetSum(n, X, a) -> true if some subset of a[0..n-1] sums to X
 * Complexity: O(n * X) time and memory
 * Status: untested
 */
bool subsetSum(int n, int X, const vector<int> &a) {
    // dp[i][j] = true if sum j can be formed using the first i elements
    vector<vector<bool>> dp(n+1, vector<bool>(X+1, false));
    dp[0][0] = true; // with 0 elements only sum 0 can be formed

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= X; j++) {
            dp[i][j] = dp[i-1][j]; // don't use a[i-1]
            if (j >= a[i-1]) {
                dp[i][j] = dp[i][j] || dp[i-1][j - a[i-1]]; // use a[i-1]
            }
        }
    }

    return dp[n][X];
}
