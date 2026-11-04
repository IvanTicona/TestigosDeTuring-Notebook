/**
 * Usage: RabinKarp(text, pat) -> positions (0-indexed) of the occurrences of pat in text; local mod/base (rolling hash)
 * Complexity: O(n + m) expected; O(n * m) worst case
 * Status: untested
 */
const ll mod = 1e9+7;
const ll base = 256; // alphabet size

vector<int> RabinKarp(const string &text, const string &pat) {
    int n = text.size();
    int m = pat.size();
    if (m > n) return {};
    vector<int> result;

    ll patHash = 0, textHash = 0, h = 1;

    // h = pow(base, m-1) % mod
    for (int i = 0; i < m-1; i++) h = (h * base) % mod;

    // hash of the pattern and of the first substring
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
