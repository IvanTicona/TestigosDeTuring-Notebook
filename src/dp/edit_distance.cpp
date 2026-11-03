#include <bits/stdc++.h>
using namespace std;

int editDistance(string a, string b) {
    int n = a.size();
    int m = b.size();
    // dp[i][j] = distancia mínima entre los primeros i caracteres de a
    //            y los primeros j caracteres de b
    vector<vector<int>> dp(n + 1, vector<int>(m + 1, 0));

    // Inicialización: convertir string vacío a otro string
    for (int i = 0; i <= n; i++) dp[i][0] = i; // eliminar todo
    for (int j = 0; j <= m; j++) dp[0][j] = j; // insertar todo

    // Llenar la tabla
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1]; // no hay costo si son iguales
            } else {
                dp[i][j] = 1 + min({
                    dp[i - 1][j],    // eliminar
                    dp[i][j - 1],    // insertar
                    dp[i - 1][j - 1] // reemplazar
                });
            }
        }
    }

    return dp[n][m];
}

int main() {
    string s1, s2;
    cout << "Introduce el primer string: ";
    cin >> s1;
    cout << "Introduce el segundo string: ";
    cin >> s2;

    cout << "La distancia de edición es: " << editDistance(s1, s2) << endl;
    return 0;
}
