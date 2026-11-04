// Stress test for src/dp/subset_sum.cpp
// Oracles: enumeration of all 2^n subsets (reachable sums and counts),
// recursion over coin multiplicities (multisets), recursion over sequences
// (ordered), BFS for the fewest coins.
#include "tests/stress/template_lib.h"
#include "src/dp/subset_sum.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_subset_sum, "src/dp/subset_sum.cpp", "example_subset_sum");
    for (stress::iter = 0; stress::iter < stress::scaled(3000); stress::iter++) {
        int n = stress::rnd(0, 12);
        vl a(n);
        ll V = stress::rnd(0, 1) ? 4 : 60;
        for (ll &x : a) x = stress::rnd(0, V);
        set<ll> reach;
        map<ll, ll> cnt;
        for (int m = 0; m < (1 << n); m++) {
            ll s = 0;
            for (int i = 0; i < n; i++) if (m >> i & 1) s += a[i];
            reach.insert(s), cnt[s]++;
        }
        auto b = subset_sums<1024>(a);
        for (int s = 0; s < 1024; s++) CHECK(b[s] == (reach.count(s) > 0), "subset_sums s=" + to_string(s));
        int S = stress::rnd(0, 200);
        CHECK(count_subsets(a, S) == cnt[S] % MOD, "count_subsets S=" + to_string(S));
        CHECK(count_subsets(a, S, 7) == cnt[S] % 7, "count_subsets mod 7");
    }
    for (stress::iter = 0; stress::iter < stress::scaled(2000); stress::iter++) {
        int k = stress::rnd(1, 4), X = stress::rnd(0, 25);
        vl c(k);
        for (ll &x : c) x = stress::rnd(1, 9);
        sort(all(c)), c.erase(unique(all(c)), c.end());
        ll ways = 0;
        auto rec = [&](auto &&self, int i, ll rem) -> void {
            if (i == sz(c)) { ways += rem == 0; return; }
            for (ll t = 0; t * c[i] <= rem; t++) self(self, i + 1, rem - t * c[i]);
        };
        rec(rec, 0, X);
        vl seq(X + 1, -1);
        auto rs = [&](auto &&self, int rem) -> ll {
            if (rem == 0) return 1;
            if (seq[rem] >= 0) return seq[rem];
            ll t = 0;
            for (ll x : c) if (x <= rem) t += self(self, rem - x);
            return seq[rem] = t;
        };
        vi dist(X + 1, -1);
        dist[0] = 0;
        queue<int> q;
        q.push(0);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (ll x : c)
                if (u + x <= X && dist[u + x] < 0) dist[u + x] = dist[u] + 1, q.push(u + x);
        }
        CHECK(coin_ways(c, X) == ways, "coin_ways X=" + to_string(X));
        CHECK(coin_seqs(c, X) == rs(rs, X) % MOD, "coin_seqs X=" + to_string(X));
        CHECK(coin_min(c, X) == dist[X], "coin_min X=" + to_string(X));
    }
    // big: many equal values (grouping) vs plain O(n S) DP
    vl a;
    for (int i = 0; i < 3000; i++) a.pb(stress::rnd(1, 30));
    auto b = subset_sums<100000>(a);
    vector<char> dp(100000, 0);
    dp[0] = 1;
    for (ll x : a)
        for (int s = 99999; s >= x; s--) dp[s] |= dp[s - x];
    for (int s = 0; s < 100000; s++) CHECK(b[s] == (dp[s] != 0), "big grouping s=" + to_string(s));
    puts("OK");
}
