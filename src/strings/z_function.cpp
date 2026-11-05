/**
 * Usage: z = z_function(s): z[i] = longest common prefix of s and s[i..]
 *        (z[0] = |s| by convention here)
 *        z_match(text, pat) -> start positions of pat in text
 * Complexity: O(n)
 * Source: UPB ZFunction + UPB isc string_z_function (wrong index in the
 *         window update) + old z_function
 * Note: z on pat + '#' + text (a separator absent from both) finds the
 *       occurrences: z[i] >= |pat|. Number of distinct substrings
 *       incrementally, string compression (smallest period p with
 *       p | n and z[p] == n - p).
 * Status: untested
 */
vi z_function(const string &s) {
    int n = sz(s);
    vi z(n, 0);
    if (n) z[0] = n;
    for (int i = 1, l = 0, r = 0; i < n; i++) {
        if (i < r) z[i] = min(r - i, z[i - l]);
        while (i + z[i] < n && s[z[i]] == s[i + z[i]]) z[i]++;
        if (i + z[i] > r) l = i, r = i + z[i];
    }
    return z;
}

vi z_match(const string &text, const string &pat) {
    vi res;
    if (pat.empty()) {  // matches at every position 0..|text|
        for (int i = 0; i <= sz(text); i++) res.pb(i);
        return res;
    }
    string s = pat + '\0' + text;  // '\0' never appears in normal input
    vi z = z_function(s);
    for (int i = 0; i + sz(pat) <= sz(text); i++)
        if (z[i + sz(pat) + 1] >= sz(pat)) res.pb(i);
    return res;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_z_function() {
    vi z = z_function("aabxaab");
    cout << z[1] << z[2] << z[3] << z[4] << '\n';  // 1003
    vi m = z_match("abcabcab", "cab");
    cout << sz(m) << ' ' << m[0] << m[1] << '\n';  // 2 25
}
