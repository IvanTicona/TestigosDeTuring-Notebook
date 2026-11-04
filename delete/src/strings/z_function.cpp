/**
 * Usage: computeZ(s) -> Z[i] = length of the longest prefix of s starting at i; for pattern search: computeZ(pat + "$" + text)
 * Complexity: O(n)
 * Status: untested
 */
// Computes the Z array of a string
vector<int> computeZ(const string &s) {
    int n = s.size();
    vector<int> Z(n, 0);
    int l = 0, r = 0; // [l, r] is the rightmost match window

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
