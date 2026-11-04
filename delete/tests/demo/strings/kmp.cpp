#include "template/template.cpp"
#include "src/strings/kmp.cpp"

int main() {
    string text = "ABABDABACDABABCABAB";
    string pat = "ABABCABAB";
    vector<int> pos = KMP(text, pat);

    cout << "Pattern found at indices: ";
    for (int p : pos) cout << p << " ";
    cout << "\n";
    return 0;
}
