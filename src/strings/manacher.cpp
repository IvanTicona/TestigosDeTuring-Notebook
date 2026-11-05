/**
 * Usage: Manacher m(s);
 *        m.d1[i] = #odd palindromes centered at i (radius incl. center)
 *        m.d2[i] = #even palindromes centered between i - 1 and i
 *        m.is_pal(l, r) in O(1); m.longest() -> {start, length};
 *        m.count() -> #palindromic substrings (by position)
 * Complexity: O(n)
 * Source: UPB Manacher (two passes d1 / d2, cp-algorithms); is_pal,
 *         longest, count: own
 * Note: distinct palindromes: eertree.cpp. Palindromic prefix of
 *       max length: is_pal(0, r) for r from n - 1 down. Min insertions to
 *       make a palindrome: |s| - LPS (DP / LCS with reverse).
 * Status: untested
 */
struct Manacher {
    vi d1, d2;
    Manacher(const string &s) : d1(sz(s)), d2(sz(s)) {
        int n = sz(s);
        for (int i = 0, l = 0, r = -1; i < n; i++) {
            int k = i > r ? 1 : min(d1[l + r - i], r - i + 1);
            while (i - k >= 0 && i + k < n && s[i - k] == s[i + k]) k++;
            d1[i] = k--;
            if (i + k > r) l = i - k, r = i + k;
        }
        for (int i = 0, l = 0, r = -1; i < n; i++) {
            int k = i > r ? 0 : min(d2[l + r - i + 1], r - i + 1);
            while (i - k - 1 >= 0 && i + k < n && s[i - k - 1] == s[i + k]) k++;
            d2[i] = k--;
            if (i + k > r) l = i - k - 1, r = i + k;
        }
    }
    bool is_pal(int l, int r) const {
        int len = r - l + 1;
        if (len <= 0) return true;
        if (len & 1) return d1[(l + r) / 2] >= (len + 1) / 2;
        return d2[(l + r + 1) / 2] >= len / 2;
    }
    pii longest() const {
        pii best = {0, 0};
        for (int i = 0; i < sz(d1); i++) {
            if (2 * d1[i] - 1 > best.S) best = {i - d1[i] + 1, 2 * d1[i] - 1};
            if (2 * d2[i] > best.S) best = {i - d2[i], 2 * d2[i]};
        }
        return best;
    }
    ll count() const {
        ll c = 0;
        for (int i = 0; i < sz(d1); i++) c += d1[i] + d2[i];
        return c;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_manacher() {
    Manacher m("abacabbac");
    auto [st, len] = m.longest();  // "cabbac" at 3
    cout << st << ' ' << len << ' '  // 3 6 10
         << m.is_pal(0, 2) << m.is_pal(0, 3) << '\n';
    cout << Manacher("aaa").count() << '\n';  // 6
}
