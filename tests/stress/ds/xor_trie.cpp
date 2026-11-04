// Stress test for src/ds/xor_trie.cpp
// Oracle: std::multiset mirror; max / min / count_less by scanning all
// elements, kth by advancing an iterator; B from 1 to 60, values at the
// top of the range, erase down to empty and refill.
#include "tests/stress/template_lib.h"
#include "src/ds/xor_trie.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_xor_trie, "src/ds/xor_trie.cpp", "example_xor_trie");
    for (stress::iter = 0; stress::iter < stress::scaled(400); stress::iter++) {
        int B = stress::rnd(0, 1) ? stress::rnd(1, 6) : stress::rnd(1, 60);
        ll M = (1LL << B) - 1;
        XorTrie t(B);
        multiset<ll> s;
        for (int op = 0; op < 200; op++) {
            if (s.empty() || stress::rnd(0, 2)) {
                ll x = stress::rnd(0, M);
                t.insert(x), s.insert(x);
            } else {
                auto it = s.begin();
                advance(it, stress::rnd(0, sz(s) - 1));
                t.erase(*it), s.erase(it);
            }
            CHECK(t.size() == sz(s), "size");
            ll x = stress::rnd(0, M), k = stress::rnd(0, 1) ? stress::rnd(0, M) : M + 1;
            ll mx = -1, mn = LLONG_MAX, cl = 0;
            for (ll y : s) mx = max(mx, x ^ y), mn = min(mn, x ^ y), cl += (x ^ y) < k;
            CHECK(t.count_less(x, k) == cl, "count_less x=" + to_string(x) + " k=" + to_string(k));
            if (s.empty()) continue;
            CHECK(t.max_xor(x) == mx && t.min_xor(x) == mn, "max/min xor");
            int kk = stress::rnd(0, sz(s) - 1);
            auto it = s.begin();
            advance(it, kk);
            CHECK(t.kth(kk) == *it, "kth");
        }
    }
    puts("OK");
}
