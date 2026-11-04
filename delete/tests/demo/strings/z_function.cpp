#include "template/template.cpp"
#include "src/strings/z_function.cpp"

int main() {
    string text = "ababcababc";
    vector<int> Z = computeZ(text);

    cout << "Z-array: ";
    for (int z : Z) cout << z << " ";
    cout << "\n";

    // Example: search for pattern "ab" in the text
    string pattern = "ab";
    string concat = pattern + "$" + text; // $ separator that does not appear in pattern
    vector<int> Z2 = computeZ(concat);

    cout << "Pattern found at indices: ";
    for (int i = 0; i < Z2.size(); i++) {
        if (Z2[i] == pattern.size())
            cout << (i - pattern.size() - 1) << " ";
    }
    cout << "\n";

    return 0;
}
