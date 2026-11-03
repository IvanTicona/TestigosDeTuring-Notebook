/**
 * Usage: subsetSum1d(X, a) -> true if some subset of a sums to X
 * Complexity: O(n * X) time, O(X) memory
 * Status: untested
 */
bool subsetSum1d(int X, const vector<int> &a) {
    vector<bool> dp(X+1, false);
    dp[0] = true; // sum 0 can always be formed

    for (int num : a) {
        for (int j = X; j >= num; j--) {
            if (dp[j - num]) dp[j] = true;
        }
    }

    return dp[X];
}
