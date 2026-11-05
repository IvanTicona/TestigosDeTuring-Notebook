// Stress test for src/strings/manacher.cpp
// Oracles: every substring checked by reversal (n <= 40): is_pal for all
// (l, r), the count of palindromic substrings, the longest length (and the
// returned start must be a palindrome of that length), d1 / d2 by direct
// expansion.
#include "tests/stress/template_lib.h"
#include "src/strings/manacher.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_manacher, "src/strings/manacher.cpp", "example_manacher");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        string s = stress::rnd_str(stress::rnd(0, 40), 'a', stress::rnd(0, 1) ? 'b' : 'd');
        int n = sz(s);
        Manacher m(s);
        ll cnt = 0;
        int best = 0;
        auto pal = [&](int l, int r) {
            for (int i = l, j = r; i < j; i++, j--) if (s[i] != s[j]) return false;
            return true;
        };
        for (int l = 0; l < n; l++)
            for (int r = l; r < n; r++) {
                bool p = pal(l, r);
                CHECK(m.is_pal(l, r) == p, "is_pal");
                cnt += p;
                if (p) best = max(best, r - l + 1);
            }
        CHECK(m.count() == cnt, "count");
        auto [st, len] = m.longest();
        CHECK(len == best && (len == 0 || pal(st, st + len - 1)), "longest");
        for (int i = 0; i < n; i++) {
            int k = 0;
            while (i - k >= 0 && i + k < n && s[i - k] == s[i + k]) k++;
            int e = 0;
            while (i - e - 1 >= 0 && i + e < n && s[i - e - 1] == s[i + e]) e++;
            CHECK(m.d1[i] == k && m.d2[i] == e, "d1 / d2");
        }
    }
    puts("OK");
}
