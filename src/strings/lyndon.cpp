/**
 * Usage: duval(s)        -> Lyndon factorization: start indices of the
 *                           factors w1 >= w2 >= ... (each w strictly
 *                           smaller than all its proper rotations)
 *        min_rotation(s) -> k such that s[k..] + s[..k) is the smallest
 *                           rotation (the smallest such k)
 * Complexity: O(n)
 * Source: UPB Booth_Duval (Booth + Duval returning substrings) + UTP
 *         minimal string rotation; via Duval on s + s: cp-algorithms
 * Note: necklace canonical form = min rotation (compare cyclic strings).
 *       Smallest suffix of s = last Lyndon factor start (after merging
 *       equal factors).
 * Status: ok
 */
vi duval(const string &s) {
    int n = sz(s), i = 0;
    vi starts;
    while (i < n) {
        int j = i + 1, k = i;
        while (j < n && s[k] <= s[j]) k = s[k] < s[j] ? i : k + 1, j++;
        while (i <= k) starts.pb(i), i += j - k;
    }
    return starts;
}

int min_rotation(const string &s) {
    int n = sz(s), i = 0, ans = 0;
    string t = s + s;
    while (i < n) {
        ans = i;
        int j = i + 1, k = i;
        while (j < 2 * n && t[k] <= t[j]) k = t[k] < t[j] ? i : k + 1, j++;
        while (i <= k) i += j - k;
    }
    return ans % max(n, 1);
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_lyndon() {
    vi f = duval("abacaba");  // abac | ab | a
    cout << sz(f) << ' ' << f[0] << f[1] << f[2] << '\n';  // 3 046
    cout << min_rotation("cabab") << ' ' << min_rotation("aaa") << '\n';  // 1 0
}
