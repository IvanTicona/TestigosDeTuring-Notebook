// Stress test for src/strings/z_function.cpp
// Oracle: O(n^2) direct comparison of s[i..] against s (Z[0] = 0 by this snippet's convention),
// and naive substring search for the `pat + "$" + text` usage.
#include "template/template.cpp"
#include "src/strings/z_function.cpp"
#include "tests/stress/common.h"

static string t_q(const string &s) { return "\"" + s + "\""; }

static vector<int> t_naive_z(const string &s) {
    int n = sz(s);
    vector<int> Z(n, 0);
    for (int i = 1; i < n; i++)
        while (i + Z[i] < n && s[Z[i]] == s[i + Z[i]]) Z[i]++;
    return Z;
}

static vector<int> t_naive_find(const string &text, const string &pat) {
    vector<int> res;
    for (int i = 0; i + sz(pat) <= sz(text); i++)
        if (text.compare(i, sz(pat), pat) == 0) res.push_back(i);
    return res;
}

// Pattern search through the Z array, as documented in the header.
static vector<int> t_search(const string &text, const string &pat) {
    int m = sz(pat);
    vector<int> Z = computeZ(pat + "$" + text), res;
    for (int i = 0; i < sz(text); i++)
        if (Z[m + 1 + i] == m) res.push_back(i);
    return res;
}

static string t_vec(const vector<int> &v) {
    string s = "[";
    for (int x : v) s += to_string(x) + " ";
    return s + "]";
}

int main() {
    // Deterministic edge cases
    CHECK(computeZ("").empty(), "empty");
    CHECK(computeZ("a") == vector<int>({0}), "single char");
    CHECK(computeZ("aaaaa") == vector<int>({0, 4, 3, 2, 1}), "all equal");
    CHECK(computeZ("abcde") == vector<int>({0, 0, 0, 0, 0}), "all distinct");
    CHECK(computeZ("aabxaab") == vector<int>({0, 1, 0, 0, 3, 1, 0}), "classic");
    CHECK(computeZ("abab") == vector<int>({0, 0, 2, 0}), "period 2");
    CHECK(t_search("aaaaa", "aa") == vector<int>({0, 1, 2, 3}), "overlapping search");
    CHECK(t_search("abc", "abcd").empty(), "pattern longer than text");
    CHECK(t_search("", "a").empty(), "empty text");
    CHECK(t_search("abcabc", "abc") == vector<int>({0, 3}), "search abc");

    for (int it = 0; it < 4000; it++) {
        stress::iter = it;
        int alpha = (int)stress::rnd(1, 4);
        int n = (int)stress::rnd(0, it % 20 == 0 ? 200 : 25);
        string s = stress::rnd_str(n, 'a', 'a' + alpha - 1);
        CHECK(computeZ(s) == t_naive_z(s), t_q(s));

        string text = stress::rnd_str(n, 'a', 'a' + alpha - 1);
        string pat = stress::rnd_str((int)stress::rnd(1, 6), 'a', 'a' + alpha - 1);
        CHECK(t_search(text, pat) == t_naive_find(text, pat), "text=" + t_q(text) + " pat=" + t_q(pat) + " got " + t_vec(t_search(text, pat)));
    }

    // Highly periodic strings (stress the [l, r] window reuse)
    for (int it = 0; it < 1000; it++) {
        stress::iter = it;
        string unit = stress::rnd_str((int)stress::rnd(1, 4), 'a', 'b');
        string s;
        int reps = (int)stress::rnd(1, 12);
        for (int k = 0; k < reps; k++) s += unit;
        if (stress::rnd(0, 1)) s += stress::rnd_str((int)stress::rnd(0, 3), 'a', 'b');
        CHECK(computeZ(s) == t_naive_z(s), t_q(s));
    }
    puts("OK");
}
