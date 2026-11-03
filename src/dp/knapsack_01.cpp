/**
 * Uso: knapsack01(n, W, w, v) con w, v indexados desde 1 (tamano n+1) -> valor maximo con capacidad W
 * Complejidad: O(n * W) tiempo y memoria
 * Status: untested
 */
int knapsack01(int n, int W, const vector<int> &w, const vector<int> &v) {
    // dp[i][j] = máximo valor con los primeros i objetos y capacidad j
    vector<vector<int>> dp(n+1, vector<int>(W+1, 0));

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= W; j++) {
            dp[i][j] = dp[i-1][j]; // no coger el objeto i
            if (j >= w[i]) {
                dp[i][j] = max(dp[i][j], dp[i-1][j - w[i]] + v[i]); // coger objeto i
            }
        }
    }

    return dp[n][W];
}
