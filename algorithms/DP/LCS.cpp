#include <bits/stdc++.h>
using namespace std;

// Función para calcular LCS
int LCS(const string &X, const string &Y) {
    int n = X.size();
    int m = Y.size();
    vector<vector<int>> dp(n+1, vector<int>(m+1, 0));

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (X[i-1] == Y[j-1])
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = max(dp[i-1][j], dp[i][j-1]);
        }
    }

    return dp[n][m];
}

// Función para reconstruir la subsecuencia
string getLCS(const string &X, const string &Y) {
    int n = X.size();
    int m = Y.size();
    vector<vector<int>> dp(n+1, vector<int>(m+1, 0));

    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++)
            dp[i][j] = (X[i-1] == Y[j-1]) ? dp[i-1][j-1]+1 : max(dp[i-1][j], dp[i][j-1]);

    string lcs = "";
    int i = n, j = m;
    while (i > 0 && j > 0) {
        if (X[i-1] == Y[j-1]) {
            lcs += X[i-1];
            i--; j--;
        } else if (dp[i-1][j] > dp[i][j-1])
            i--;
        else
            j--;
    }
    reverse(lcs.begin(), lcs.end());
    return lcs;
}

int main() {
    string X = "AGGTAB";
    string Y = "GXTXAYB";

    cout << "Length of LCS: " << LCS(X, Y) << "\n";
    cout << "LCS: " << getLCS(X, Y) << "\n";

    return 0;
}
