// Stress test for src/strings/rabin_karp.cpp
// Oracle: naive O(n*m) substring comparison.
//
// KNOWN BUG: with an empty pattern and a non-empty text, RabinKarp() returns only
//   {0} instead of every position 0..n: the rolling update stops matching the
//   (zero) pattern hash after the first step. The test uses m >= 1 (plus the
//   degenerate text == pat == "" case, which works).
// KNOWN BUG: `pat[i]` / `text[i]` are (signed) chars, so bytes >= 128 are negative.
//   The initial hashes and the rolling hash then land in different residue
//   representatives (negative vs non-negative) and real matches are missed
//   (e.g. text = 5 x byte 0xFF, pat = 1 x byte 0xFF returns {}). The test uses
//   ASCII bytes (1..127) only.
#include "template/template.cpp"
#include "src/strings/rabin_karp.cpp"
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

static void t_check(const string &text, const string &pat) {
    vector<int> got = RabinKarp(text, pat);
    CHECK(got == t_naive_find(text, pat), "text=" + t_q(text) + " pat=" + t_q(pat) + " got " + t_vec(got));
}

int main() {
    // Deterministic edge cases
    t_check("", "a");
    t_check("abc", "abcd");
    t_check("abc", "abc");
    t_check("aaaaa", "aa");
    t_check("abababab", "abab");
    t_check("abcabc", "c");
    t_check("abc", "d");
    t_check("", "");        // m = 0, n = 0 -> {0} (m = 0 with n > 0 is the KNOWN BUG above)

    // Random, ASCII letters
    for (int it = 0; it < 5000; it++) {
        stress::iter = it;
        int alpha = (int)stress::rnd(1, 4);
        string text = stress::rnd_str((int)stress::rnd(0, it % 20 == 0 ? 200 : 25), 'a', 'a' + alpha - 1);
        string pat = stress::rnd_str((int)stress::rnd(1, 8), 'a', 'a' + alpha - 1);
        t_check(text, pat);
    }

    // Random, full ASCII range 1..127 (hash values up to base^m wrap mod 1e9+7)
    for (int it = 0; it < 3000; it++) {
        stress::iter = it;
        auto gen = [&](int n) {
            string s(n, 0);
            for (auto &c : s) c = (char)(stress::rnd(0, 1) ? stress::rnd(1, 127) : stress::rnd(1, 3));
            return s;
        };
        string text = gen((int)stress::rnd(0, it % 20 == 0 ? 200 : 25));
        string pat = gen((int)stress::rnd(1, 6));
        t_check(text, pat);
    }
    puts("OK");
}
