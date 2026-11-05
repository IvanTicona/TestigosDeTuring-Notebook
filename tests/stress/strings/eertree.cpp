// Stress test for src/strings/eertree.cpp
// Oracles: set of all palindromic substrings (distinct count), every node
// is a distinct palindrome of length len[v] whose overlapping occurrence
// count is occ[v]; ends[i] = #palindromic substrings ending at i.
#include "tests/stress/template_lib.h"
#include "src/strings/eertree.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_eertree, "src/strings/eertree.cpp", "example_eertree");
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        string s = stress::rnd_str(stress::rnd(0, 40), 'a', stress::rnd(0, 1) ? 'b' : 'c');
        int n = sz(s);
        auto pal = [](const string &t) { return t == string(rall(t)); };
        set<string> ps;
        vi endc(n, 0);
        for (int i = 0; i < n; i++)
            for (int j = i; j < n; j++)
                if (pal(s.substr(i, j - i + 1))) ps.insert(s.substr(i, j - i + 1)), endc[j]++;
        Eertree e(s);
        CHECK(e.distinct() == sz(ps), "distinct");
        CHECK(e.ends == endc, "ends");
        set<string> seen;
        for (int v = 2; v < sz(e.len); v++) {
            string t = s.substr(e.pos[v] - e.len[v] + 1, e.len[v]);
            CHECK(pal(t) && seen.insert(t).second, "node is a new palindrome");
            ll c = 0;
            for (int i = 0; i + sz(t) <= n; i++) c += s.compare(i, sz(t), t) == 0;
            CHECK(e.occ[v] == c, "occurrences of " + t);
        }
    }
    puts("OK");
}
