#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
const ll mod = 1e9+7;
const ll base = 256; // tamaño del alfabeto

vector<int> RabinKarp(const string &text, const string &pat) {
    int n = text.size();
    int m = pat.size();
    if (m > n) return {};
    vector<int> result;

    ll patHash = 0, textHash = 0, h = 1;

    // h = pow(base, m-1) % mod
    for (int i = 0; i < m-1; i++) h = (h * base) % mod;

    // hash del patrón y del primer substring
    for (int i = 0; i < m; i++) {
        patHash = (base * patHash + pat[i]) % mod;
        textHash = (base * textHash + text[i]) % mod;
    }

    for (int i = 0; i <= n - m; i++) {
        if (patHash == textHash) {
            if (text.substr(i, m) == pat) {
                result.push_back(i);
            }
        }
        if (i < n - m) {
            textHash = (base * (textHash - text[i] * h) + text[i + m]) % mod;
            if (textHash < 0) textHash += mod;
        }
    }

    return result;
}

int main() {
    string text = "ABABDABACDABABCABAB";
    string pat = "ABABCABAB";
    vector<int> pos = RabinKarp(text, pat);

    cout << "Pattern found at indices: ";
    for (int p : pos) cout << p << " ";
    cout << "\n";
    return 0;
}
