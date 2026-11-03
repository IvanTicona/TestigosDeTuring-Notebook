/**
 * Usage: knapsack01_1d(n, W, w, v) con w, v indexados desde 0 (tamano n) -> valor maximo con capacidad W
 * Complexity: O(n * W) tiempo, O(W) memoria
 * Status: untested
 */
int knapsack01_1d(int n, int W, const vector<int> &w, const vector<int> &v) {
    vector<int> dp(W+1, 0);

    for (int i = 0; i < n; i++) {
        for (int j = W; j >= w[i]; j--) {
            dp[j] = max(dp[j], dp[j - w[i]] + v[i]);
        }
    }

    return dp[W];
}
