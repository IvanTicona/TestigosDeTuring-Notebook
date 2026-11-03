/**
 * Uso: subsetSum1d(X, a) -> true si algun subconjunto de a suma X
 * Complejidad: O(n * X) tiempo, O(X) memoria
 * Status: untested
 */
bool subsetSum1d(int X, const vector<int> &a) {
    vector<bool> dp(X+1, false);
    dp[0] = true; // siempre se puede formar suma 0

    for (int num : a) {
        for (int j = X; j >= num; j--) {
            if (dp[j - num]) dp[j] = true;
        }
    }

    return dp[X];
}
