// Stress test for src/strings/suffix_automaton.cpp
// Oracles: std::set of all substrings (distinct count), direct occurrence
// counting with overlaps, longest common substring by checking all
// substrings of t; the number of states stays <= 2n.
#include "tests/stress/template_lib.h"
#include "src/strings/suffix_automaton.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_suffix_automaton, "src/strings/suffix_automaton.cpp", "example_suffix_automaton");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        char hi = stress::rnd(0, 1) ? 'b' : 'e';
        string s = stress::rnd_str(stress::rnd(0, 30), 'a', hi), t = stress::rnd_str(stress::rnd(0, 20), 'a', hi);
        SAM sam(s);
        CHECK(sz(sam.len) <= max(2, 2 * sz(s)), "states");
        set<string> subs;
        for (int i = 0; i < sz(s); i++) for (int j = i; j < sz(s); j++) subs.insert(s.substr(i, j - i + 1));
        CHECK(sam.distinct() == sz(subs), "distinct");
        for (int q = 0; q < 10; q++) {
            string p = stress::rnd_str(stress::rnd(1, 4), 'a', hi);
            if (stress::rnd(0, 1) && !s.empty()) {
                int i = stress::rnd(0, sz(s) - 1);
                p = s.substr(i, stress::rnd(1, sz(s) - i));
            }
            ll c = 0;
            for (int i = 0; i + sz(p) <= sz(s); i++) c += s.compare(i, sz(p), p) == 0;
            CHECK(sam.occ(p) == c, "occ " + p);
        }
        int best = 0;
        for (int i = 0; i < sz(t); i++)
            for (int j = i; j < sz(t); j++)
                if (subs.count(t.substr(i, j - i + 1))) best = max(best, j - i + 1);
        CHECK(sam.lcs(t) == best, "lcs s=" + s + " t=" + t);
    }
    puts("OK");
}
