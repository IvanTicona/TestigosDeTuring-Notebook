/**
 * Usage: knapsack01(n, W, w, v) with w, v 1-indexed (size n+1) -> maximum value with capacity W
 * Complexity: O(n * W) time and memory
 * Status: untested
 */
int knapsack01(int n, int W, const vector<int> &w, const vector<int> &v) {
    // dp[i][j] = maximum value using the first i items with capacity j
    vector<vector<int>> dp(n+1, vector<int>(W+1, 0));

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= W; j++) {
            dp[i][j] = dp[i-1][j]; // skip item i
            if (j >= w[i]) {
                dp[i][j] = max(dp[i][j], dp[i-1][j - w[i]] + v[i]); // take item i
            }
        }
    }

    return dp[n][W];
}
