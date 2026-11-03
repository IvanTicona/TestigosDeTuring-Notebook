#include <bits/stdc++.h>
using namespace std;

// Función que calcula el array Z de una cadena
vector<int> computeZ(const string &s) {
    int n = s.size();
    vector<int> Z(n, 0);
    int l = 0, r = 0; // [l, r] es la ventana de coincidencia más a la derecha

    for (int i = 1; i < n; i++) {
        if (i <= r)
            Z[i] = min(r - i + 1, Z[i - l]);

        while (i + Z[i] < n && s[Z[i]] == s[i + Z[i]])
            Z[i]++;

        if (i + Z[i] - 1 > r) {
            l = i;
            r = i + Z[i] - 1;
        }
    }
    return Z;
}

int main() {
    string text = "ababcababc";
    vector<int> Z = computeZ(text);

    cout << "Z-array: ";
    for (int z : Z) cout << z << " ";
    cout << "\n";

    // Ejemplo: buscar patrón "ab" en el texto
    string pattern = "ab";
    string concat = pattern + "$" + text; // $ separador que no aparece en pattern
    vector<int> Z2 = computeZ(concat);

    cout << "Pattern found at indices: ";
    for (int i = 0; i < Z2.size(); i++) {
        if (Z2[i] == pattern.size())
            cout << (i - pattern.size() - 1) << " ";
    }
    cout << "\n";

    return 0;
}
