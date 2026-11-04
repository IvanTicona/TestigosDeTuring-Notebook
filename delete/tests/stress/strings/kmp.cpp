// Stress test for src/strings/kmp.cpp
// Oracle: naive O(n*m) substring comparison, and a naive longest proper
// border computation for the LPS array.
//
// KNOWN BUG: an empty pattern makes KMP() evaluate `lps[j - 1]` with j = 0
//   (out of bounds). The test uses patterns with m >= 1.
#include "template/template.cpp"
#include "src/strings/kmp.cpp"
#include "tests/stress/common.h"

static string t_q(const string &s) { return "\"" + s + "\""; }

static string t_vec(const vector<int> &v) {
    string s = "[";
    for (int x : v) s += to_string(x) + " ";
    return s + "]";
}

static vector<int> t_naive_find(const string &text, const string &pat) {
    vector<int> res;
    for (int i = 0; i + sz(pat) <= sz(text); i++)
        if (text.compare(i, sz(pat), pat) == 0) res.push_back(i);
    return res;
}

// lps[i] = length of the longest proper prefix of pat[0..i] that is also its suffix.
static vector<int> t_naive_lps(const string &pat) {
    int m = sz(pat);
    vector<int> lps(m, 0);
    for (int i = 0; i < m; i++)
        for (int k = i; k >= 1; k--)
            if (pat.compare(0, k, pat, i + 1 - k, k) == 0) { lps[i] = k; break; }
    return lps;
}

int main() {
    // Deterministic edge cases
    CHECK(computeLPS("").empty(), "lps empty");
    CHECK(computeLPS("a") == vector<int>({0}), "lps single");
    CHECK(computeLPS("aabaaab") == vector<int>({0, 1, 0, 1, 2, 2, 3}), "lps classic");
    CHECK(computeLPS("aaaa") == vector<int>({0, 1, 2, 3}), "lps all equal");
    CHECK(KMP("", "a").empty(), "empty text");
    CHECK(KMP("abc", "abcd").empty(), "m > n");
    CHECK(KMP("abc", "abc") == vector<int>({0}), "text == pat");
    CHECK(KMP("aaaaa", "aa") == vector<int>({0, 1, 2, 3}), "overlapping");
    CHECK(KMP("abababab", "abab") == vector<int>({0, 2, 4}), "period 2");
    CHECK(KMP("abcabc", "c") == vector<int>({2, 5}), "single char pattern");
    CHECK(KMP("abc", "d").empty(), "no match");

    for (int it = 0; it < 5000; it++) {
        stress::iter = it;
        int alpha = (int)stress::rnd(1, 4);
        int n = (int)stress::rnd(0, it % 20 == 0 ? 200 : 25);
        string text = stress::rnd_str(n, 'a', 'a' + alpha - 1);
        string pat = stress::rnd_str((int)stress::rnd(1, 8), 'a', 'a' + alpha - 1);
        string in = "text=" + t_q(text) + " pat=" + t_q(pat);
        CHECK(computeLPS(pat) == t_naive_lps(pat), in + " lps got " + t_vec(computeLPS(pat)));
        CHECK(KMP(text, pat) == t_naive_find(text, pat), in + " got " + t_vec(KMP(text, pat)));
    }

    // Periodic texts and patterns (many overlapping matches)
    for (int it = 0; it < 1000; it++) {
        stress::iter = it;
        string unit = stress::rnd_str((int)stress::rnd(1, 3), 'a', 'b');
        string text, pat;
        for (int k = (int)stress::rnd(1, 15); k > 0; k--) text += unit;
        for (int k = (int)stress::rnd(1, 5); k > 0; k--) pat += unit;
        if (stress::rnd(0, 1)) pat += stress::rnd_str((int)stress::rnd(0, 2), 'a', 'b');
        string in = "text=" + t_q(text) + " pat=" + t_q(pat);
        CHECK(KMP(text, pat) == t_naive_find(text, pat), in);
        CHECK(computeLPS(pat) == t_naive_lps(pat), in);
    }
    puts("OK");
}
