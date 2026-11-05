// Stress test for src/strings/prefix_function.cpp
// Oracles: borders by checking every length; occurrences by comparing at
// every position (empty pattern, pattern longer than text, small and full
// alphabets); period by definition; automaton states = longest suffix of
// (prefix + c) that is a prefix of s, computed by brute force.
#include "tests/stress/template_lib.h"
#include "src/strings/prefix_function.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_prefix_function, "src/strings/prefix_function.cpp", "example_prefix_function");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        char hi = stress::rnd(0, 1) ? 'b' : 'z';
        string s = stress::rnd_str(stress::rnd(0, 30), 'a', hi), t = stress::rnd_str(stress::rnd(0, 40), 'a', hi);
        if (stress::rnd(0, 3) == 0) t = s + t + s;
        vi pi = prefix_function(s);
        for (int i = 0; i < sz(s); i++) {
            int b = 0;
            for (int k = 1; k <= i; k++) if (s.substr(0, k) == s.substr(i + 1 - k, k)) b = k;
            CHECK(pi[i] == b, "pi");
        }
        vi want;
        for (int i = 0; i + sz(s) <= sz(t); i++) if (t.compare(i, sz(s), s) == 0) want.pb(i);
        CHECK(kmp(t, s) == want, "kmp text=" + t + " pat=" + s);
        if (!s.empty()) {
            int p = 1;
            while (p < sz(s)) {
                bool ok = true;
                for (int i = 0; i + p < sz(s); i++) ok &= s[i] == s[i + p];
                if (ok) break;
                p++;
            }
            CHECK(period(s) == p, "period");
        }
        if (!s.empty() && hi == 'b') {
            auto aut = kmp_automaton(s, 'a', 2);
            for (int i = 0; i <= sz(s); i++)
                for (int c = 0; c < 2; c++) {
                    string x = s.substr(0, i) + char('a' + c);
                    int best = 0;
                    for (int k = 1; k <= min(sz(x), sz(s)); k++)
                        if (x.substr(sz(x) - k) == s.substr(0, k)) best = k;
                    CHECK(aut[i][c] == best, "automaton");
                }
        }
    }
    puts("OK");
}
