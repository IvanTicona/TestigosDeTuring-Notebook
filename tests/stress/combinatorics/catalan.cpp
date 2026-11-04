// Stress test for src/combinatorics/catalan.cpp
// Oracles: Catalan by the convolution recurrence in exact __int128 (n <= 60)
// and in modular DP; lattice-path DP (step by step, height >= 0) for
// paths_nonneg; brute-force enumeration of bracket strings with a prefix;
// Narayana by enumerating Dyck words and counting peaks "()" (n <= 12).
#include "tests/stress/template_lib.h"
#include "src/number_theory/modular_arithmetic.cpp"
#include "src/combinatorics/binomial.cpp"
#include "src/combinatorics/catalan.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_catalan, "src/combinatorics/catalan.cpp", "example_catalan");
    Comb cb(4000);
    // exact Catalan up to 60 (C_60 ~ 5e32 fits in unsigned __int128)
    vector<unsigned __int128> ex(61, 0);
    ex[0] = 1;
    for (int n = 0; n < 60; n++) for (int i = 0; i <= n; i++) ex[n + 1] += ex[i] * ex[n - i];
    for (int n = 0; n <= 60; n++) CHECK(catalan(cb, n) == (ll)(ex[n] % MOD), "C_" + to_string(n));
    CHECK(catalan(cb, -1) == 0, "C_-1");
    // modular recurrence up to 1900 vs closed form
    vl rec(1901, 0);
    rec[0] = 1;
    for (int n = 0; n < 1900; n++) for (int i = 0; i <= n; i++) rec[n + 1] = (rec[n + 1] + rec[i] * rec[n - i]) % MOD;
    for (int n = 0; n <= 1900; n++) CHECK(catalan(cb, n) == rec[n], "rec C_" + to_string(n));
    // catalan_any vs exact for several moduli
    for (ll m : {1LL, 2LL, 10LL, 1000LL, 720720LL, (ll)4e18}) {
        vl c = catalan_any(60, m);
        for (int n = 0; n <= 60; n++) CHECK(c[n] == (ll)(ex[n] % m), "any C_" + to_string(n) + " m=" + to_string(m));
    }
    // paths_nonneg vs DP
    const int L = 60;
    for (int x = 0; x <= 20; x++) {
        vl cur(L + 2, 0);  // cur[h] = #ways to be at height h
        cur[x] = 1;
        for (int l = 0; l <= L; l++) {
            for (int y = 0; y <= 20; y++)
                CHECK(paths_nonneg(cb, l, x, y) == cur[y] % MOD,
                      "L=" + to_string(l) + " x=" + to_string(x) + " y=" + to_string(y));
            vl nx(L + 2, 0);
            for (int h = 0; h <= L; h++) {
                if (h + 1 <= L) nx[h + 1] = (nx[h + 1] + cur[h]) % MOD;
                if (h - 1 >= 0) nx[h - 1] = (nx[h - 1] + cur[h]) % MOD;
            }
            cur = nx;
        }
    }
    CHECK(paths_nonneg(cb, 5, -1, 0) == 0 && paths_nonneg(cb, 3, 0, 0) == 0, "invalid paths");
    // brackets_with_prefix vs enumeration (n <= 14)
    for (int n = 0; n <= 14; n++) {
        map<string, ll> cnt;  // prefix (length <= 4) -> #valid strings
        for (int mask = 0; mask < (1 << n); mask++) {
            string t;
            int h = 0;
            bool ok = true;
            for (int i = 0; i < n; i++) {
                t += (mask >> i & 1) ? '(' : ')';
                h += t.back() == '(' ? 1 : -1;
                ok &= h >= 0;
            }
            if (!ok || h != 0) continue;
            for (int len = 0; len <= min(n, 4); len++) cnt[t.substr(0, len)]++;
        }
        for (int len = 0; len <= 5; len++)
            for (int mask = 0; mask < (1 << len); mask++) {
                string pre;
                for (int i = 0; i < len; i++) pre += (mask >> i & 1) ? '(' : ')';
                ll want = len <= min(n, 4) ? cnt[pre] : -1;
                if (want < 0) continue;
                CHECK(brackets_with_prefix(cb, n, pre) == want, "n=" + to_string(n) + " prefix=" + pre);
            }
        CHECK(brackets_with_prefix(cb, n, string(n + 1, '(')) == 0, "prefix longer than n");
    }
    // Narayana vs peak counting, and sum over k == Catalan
    for (int n = 1; n <= 12; n++) {
        vl peaks(n + 1, 0);
        for (int mask = 0; mask < (1 << (2 * n)); mask++) {
            int h = 0, pk = 0;
            bool ok = true;
            for (int i = 0; i < 2 * n && ok; i++) {
                bool up = mask >> i & 1;
                if (!up && i > 0 && (mask >> (i - 1) & 1)) pk++;
                h += up ? 1 : -1;
                ok = h >= 0;
            }
            if (ok && h == 0) peaks[pk]++;
        }
        ll s = 0;
        for (int k = 0; k <= n + 1; k++) {
            ll want = k <= n ? peaks[k] : 0;
            CHECK(narayana(cb, n, k) == want % MOD, "N(" + to_string(n) + "," + to_string(k) + ")");
            s += narayana(cb, n, k);
        }
        CHECK(s % MOD == catalan(cb, n), "sum Narayana n=" + to_string(n));
    }
    puts("OK");
}
