// Stress test for src/dp/lcs.cpp
// Oracle: enumerate every subsequence of X (n <= 12) and test it against Y.
#include "template/template.cpp"
#include "src/dp/lcs.cpp"
#include "tests/stress/common.h"

static string t_q(const string &s) { return "\"" + s + "\""; }

static bool t_is_subseq(const string &sub, const string &s) {
    int j = 0;
    for (char c : s) if (j < sz(sub) && sub[j] == c) j++;
    return j == sz(sub);
}

static int t_brute(const string &X, const string &Y) {
    int n = sz(X), best = 0;
    for (int mask = 0; mask < (1 << n); mask++) {
        string sub;
        for (int i = 0; i < n; i++) if (mask >> i & 1) sub += X[i];
        if (sz(sub) > best && t_is_subseq(sub, Y)) best = sz(sub);
    }
    return best;
}

int main() {
    // Deterministic edge cases
    CHECK(LCS("", "") == 0 && getLCS("", "") == "", "'' ''");
    CHECK(LCS("abc", "") == 0 && getLCS("abc", "") == "", "abc ''");
    CHECK(LCS("", "abc") == 0 && getLCS("", "abc") == "", "'' abc");
    CHECK(LCS("abc", "abc") == 3 && getLCS("abc", "abc") == "abc", "abc abc");
    CHECK(LCS("abc", "xyz") == 0 && getLCS("abc", "xyz") == "", "abc xyz");
    CHECK(LCS("AGGTAB", "GXTXAYB") == 4 && getLCS("AGGTAB", "GXTXAYB") == "GTAB", "AGGTAB GXTXAYB");
    CHECK(LCS("aaaa", "aa") == 2, "aaaa aa");

    for (int it = 0; it < 3000; it++) {
        stress::iter = it;
        int alpha = (int)stress::rnd(1, 4);
        string X = stress::rnd_str((int)stress::rnd(0, 12), 'a', 'a' + alpha - 1);
        string Y = stress::rnd_str((int)stress::rnd(0, 14), 'a', 'a' + alpha - 1);
        string in = t_q(X) + " " + t_q(Y);
        int want = t_brute(X, Y);
        CHECK(LCS(X, Y) == want, in);
        string g = getLCS(X, Y);
        CHECK(sz(g) == want, in);
        CHECK(t_is_subseq(g, X) && t_is_subseq(g, Y), in);
    }
    puts("OK");
}
