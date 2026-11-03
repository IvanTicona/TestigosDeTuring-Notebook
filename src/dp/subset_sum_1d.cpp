#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, X;
    cin >> n >> X;

    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    vector<bool> dp(X+1, false);
    dp[0] = true; // siempre se puede formar suma 0

    for (int num : a) {
        for (int j = X; j >= num; j--) {
            if (dp[j - num]) dp[j] = true;
        }
    }

    if (dp[X]) cout << "Sí, existe un subconjunto con suma " << X << "\n";
    else cout << "No existe subconjunto con suma " << X << "\n";
}
