/**
 * Usage: editDistance(a, b) -> minimum number of insertions/deletions/replacements to turn a into b
 * Complexity: O(n * m) time and memory
 * Status: untested
 */
int editDistance(string a, string b) {
    int n = a.size();
    int m = b.size();
    // dp[i][j] = minimum distance between the first i characters of a
    //            and the first j characters of b
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

    // Initialization: convert an empty string to another string
    for (int i = 0; i <= n; i++) dp[i][0] = i; // delete everything
    for (int j = 0; j <= m; j++) dp[0][j] = j; // insert everything

    // Fill the table
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1]; // no cost if they are equal
            } else {
                dp[i][j] = 1 + min({
                    dp[i - 1][j],    // delete
                    dp[i][j - 1],    // insert
                    dp[i - 1][j - 1] // replace
                });
            }
        }
    }

    return dp[n][m];
}
