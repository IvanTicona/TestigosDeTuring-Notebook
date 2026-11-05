// Stress test for src/strings/suffix_array.cpp
// Oracles: sort all suffixes with std::string comparison (bytes 0..255
// including '\0', periodic strings like "aaaa", "abab"); LCP by direct
// comparison; distinct substrings via a std::set (n <= 40); a 2e5 random
// string checked for sortedness on sampled adjacent pairs.
#include "tests/stress/template_lib.h"
#include "src/strings/suffix_array.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_suffix_array, "src/strings/suffix_array.cpp", "example_suffix_array");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int n = stress::rnd(0, 40), mode = stress::rnd(0, 3);
        string s(n, 'a');
        for (int i = 0; i < n; i++)
            s[i] = mode == 0 ? 'a' + stress::rnd(0, 1) : mode == 1 ? (char)stress::rnd(0, 255) : mode == 2 ? 'a' + i % 2 : 'a';
        vi want(n);
        iota(all(want), 0);
        sort(all(want), [&](int a, int b) { return s.substr(a) < s.substr(b); });
        vi sa = suffix_array(s);
        CHECK(sa == want, "suffix array mode=" + to_string(mode));
        vi lcp = lcp_array(s, sa);
        CHECK(sz(lcp) == max(0, n - 1), "lcp size");
        for (int i = 0; i + 1 < n; i++) {
            int k = 0;
            while (sa[i] + k < n && sa[i + 1] + k < n && s[sa[i] + k] == s[sa[i + 1] + k]) k++;
            CHECK(lcp[i] == k, "lcp");
        }
        set<string> subs;
        for (int i = 0; i < n; i++) for (int j = i; j < n; j++) subs.insert(s.substr(i, j - i + 1));
        CHECK((ll)n * (n + 1) / 2 - accumulate(all(lcp), 0LL) == sz(subs), "distinct substrings");
    }
    int n = stress::SLOW ? 30000 : 200000;
    string s = stress::rnd_str(n, 'a', 'c');
    vi sa = suffix_array(s);
    for (int t = 0; t < 200; t++) {
        int i = stress::rnd(0, n - 2);
        CHECK(s.compare(sa[i], string::npos, s, sa[i + 1], string::npos) < 0, "big sorted");
    }
    puts("OK");
}
