/**
 * Usage: Hash h(s);  h.get(l, r) -> hash of s[l..r] (inclusive, l > r: 0)
 *        equal substrings <=> equal hashes (w.h.p.), also across strings
 *        hashed with the same (static random) base
 *        lcp(a, i, b, j) -> longest common prefix of a[i..] and b[j..]
 *        (compare suffixes / substrings lexicographically in O(log n))
 * Complexity: O(n) build, O(1) get, O(log n) lcp
 * Source: UMSS Fast Hashing (two fixed primes, fixed bases: hackable) +
 *         UPB RollingHash + old rabin_karp (empty pattern; signed chars >=
 *         128 hashed as negative); mod 2^61 - 1 with a random base: own
 * Note: collision probability ~ n / 2^61 per comparison. Palindrome check:
 *       hash of s and of reversed s. Multiset hashing: sum of rnd(x).
 *       2D: hash rows, then hash the column of row hashes (other base).
 * Status: untested
 */
struct Hash {
    static constexpr ull M = (1ULL << 61) - 1;
    static inline ull B = rng() % (M - 1000) + 500;  // random base
    static ull mul(ull a, ull b) {
        unsigned __int128 c = (unsigned __int128)a * b;
        ull r = (ull)(c & M) + (ull)(c >> 61);
        return r >= M ? r - M : r;
    }
    vector<ull> h, pw;
    Hash(const string &s) : h(sz(s) + 1, 0), pw(sz(s) + 1, 1) {
        for (int i = 0; i < sz(s); i++) {
            h[i + 1] = mul(h[i], B) + (unsigned char)s[i] + 1;
            if (h[i + 1] >= M) h[i + 1] -= M;
            pw[i + 1] = mul(pw[i], B);
        }
    }
    ull get(int l, int r) const {
        if (l > r) return 0;
        ull x = h[r + 1] + M - mul(h[l], pw[r - l + 1]);
        return x >= M ? x - M : x;
    }
};

int lcp(const Hash &a, int i, const Hash &b, int j) {
    int lo = 0, hi = min(sz(a.h) - 1 - i, sz(b.h) - 1 - j);
    while (lo < hi) {
        int md = (lo + hi + 1) / 2;
        if (a.get(i, i + md - 1) == b.get(j, j + md - 1)) lo = md;
        else hi = md - 1;
    }
    return lo;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_hashing() {
    string s = "abracadabra";
    Hash h(s);
    bool same = h.get(0, 3) == h.get(7, 10), diff = h.get(0, 2) == h.get(1, 3);
    // "abra" twice; lcp("abracadabra", "abra") = 4, ("acadabra", "adabra") = 1
    cout << same << diff << ' ' << lcp(h, 0, h, 7) << ' '  // 10 4 1
         << lcp(h, 3, h, 5) << '\n';
}
