#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, X;
    cin >> n >> X; // número de elementos y suma objetivo

    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

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

    if (dp[n][X]) cout << "Sí, existe un subconjunto con suma " << X << "\n";
    else cout << "No existe subconjunto con suma " << X << "\n";
}
