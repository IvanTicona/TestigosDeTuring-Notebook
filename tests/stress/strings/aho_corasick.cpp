// Stress test for src/strings/aho_corasick.cpp
// Oracles: overlapping occurrence counts by comparing every pattern at
// every position (duplicate patterns, patterns that are suffixes /
// prefixes of others, tiny alphabets); transitions = longest suffix of
// (node string + c) that is a trie prefix, checked by brute force.
#include "tests/stress/template_lib.h"
#include "src/strings/aho_corasick.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_aho_corasick, "src/strings/aho_corasick.cpp", "example_aho_corasick");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        char hi = stress::rnd(0, 1) ? 'b' : 'd';
        int k = stress::rnd(1, 8);
        vector<string> ps(k);
        for (auto &p : ps) p = stress::rnd_str(stress::rnd(1, 5), 'a', hi);
        if (stress::rnd(0, 2) == 0) ps.pb(ps[0]);  // duplicate
        string t = stress::rnd_str(stress::rnd(0, 60), 'a', hi);
        Aho ac;
        for (auto &p : ps) ac.add(p);
        ac.build();
        vl got = ac.count(t);
        for (int i = 0; i < sz(ps); i++) {
            ll c = 0;
            for (int j = 0; j + sz(ps[i]) <= sz(t); j++) c += t.compare(j, sz(ps[i]), ps[i]) == 0;
            CHECK(got[i] == c, "count pattern " + ps[i]);
        }
        // transitions: map node -> its string, then brute force
        vector<string> str(sz(ac.go));
        set<string> prefixes;
        for (auto &p : ps) for (int l = 0; l <= sz(p); l++) prefixes.insert(p.substr(0, l));
        vi q = {0};
        vector<char> vis(sz(ac.go), 0);
        vis[0] = 1;
        for (int i = 0; i < sz(q); i++)
            for (int c = 0; c <= hi - 'a'; c++) {
                int u = ac.go[q[i]][c];
                string want = str[q[i]] + char('a' + c);
                while (!prefixes.count(want)) want = want.substr(1);
                if (!vis[u]) vis[u] = 1, str[u] = want, q.pb(u);
                CHECK(str[u] == want, "transition");
            }
    }
    puts("OK");
}
