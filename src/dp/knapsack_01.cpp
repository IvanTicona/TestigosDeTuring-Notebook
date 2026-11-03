#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, W;
    cin >> n >> W;  // n = número de objetos, W = capacidad máxima

    vector<int> w(n+1), v(n+1);
    for (int i = 1; i <= n; i++) {
        cin >> w[i] >> v[i]; // peso y valor de cada objeto
    }

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

    cout << "Valor máximo = " << dp[n][W] << "\n";
}
