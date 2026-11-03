#include "template/template.cpp"
#include "src/strings/z_function.cpp"

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
