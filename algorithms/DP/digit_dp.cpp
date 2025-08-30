#include <bits/stdc++.h>
using namespace std;

string s; // número N como string
int n;
int dp[20][200][2][2]; 
// pos, suma, tight, started

// retorna cantidad de números válidos desde la posición pos
int solve(int pos, int sum, bool tight, bool started) {
    if (pos == n) {
        // condición final: suma divisible por 3
        return (started && sum % 3 == 0);
    }

    int &res = dp[pos][sum][tight][started];
    if (res != -1) return res;
    res = 0;

    int limit = tight ? (s[pos] - '0') : 9;

    for (int dig = 0; dig <= limit; dig++) {
        bool nextTight = tight && (dig == limit);
        bool nextStarted = started || (dig != 0);
        int nextSum = sum;
        if (nextStarted) nextSum += dig;

        res += solve(pos + 1, nextSum, nextTight, nextStarted);
    }
    return res;
}

int main() {
    long long N;
    cin >> N;
    s = to_string(N);
    n = s.size();
    memset(dp, -1, sizeof(dp));
    cout << "Cantidad de números con suma de dígitos divisible por 3 en [0," << N << "] = "
         << solve(0, 0, true, false) << "\n";
}
