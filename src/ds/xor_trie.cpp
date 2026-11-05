/**
 * Usage: XorTrie t(B);  numbers in [0, 2^B), multiset
 *        t.insert(x), t.erase(x) (x must be present), t.size()
 *        t.max_xor(x) / t.min_xor(x) -> max / min of x ^ y over y in set
 *        t.count_less(x, k)          -> #y in set with (x ^ y) < k
 *        t.kth(k)                    -> k-th smallest element (0-indexed)
 * Complexity: O(B) per operation, B * #inserts nodes
 * Source: UPB BinaryTrieXOR (+ min xor pair by sorting) + UMSS Xor Trie +
 *         UTP trie; count_less / kth: own
 * Note: max xor of a subarray: insert prefix xors, query each prefix.
 *       Min xor over all pairs = min over adjacent pairs of the sorted
 *       array. Persistent version: clone nodes on insert (like PST).
 * Status: ok
 */
struct XorTrie {
    int B;
    vector<array<int, 2>> ch;
    vi cnt;  // #numbers through the node
    XorTrie(int B_ = 30) : B(B_), ch(1, {0, 0}), cnt(1, 0) {}
    void upd(ll x, int d) {
        int u = 0;
        cnt[0] += d;
        for (int b = B - 1; b >= 0; b--) {
            int c = x >> b & 1;
            if (!ch[u][c]) ch[u][c] = sz(cnt), ch.pb({0, 0}), cnt.pb(0);
            u = ch[u][c], cnt[u] += d;
        }
    }
    void insert(ll x) { upd(x, 1); }
    void erase(ll x) { upd(x, -1); }
    int size() const { return cnt[0]; }
    bool has(int u) const { return u && cnt[u] > 0; }
    ll max_xor(ll x) const {  // set must be non-empty
        int u = 0;
        ll r = 0;
        for (int b = B - 1; b >= 0; b--) {
            int c = x >> b & 1;
            if (has(ch[u][c ^ 1])) r |= 1LL << b, u = ch[u][c ^ 1];
            else u = ch[u][c];
        }
        return r;
    }
    ll min_xor(ll x) const {  // M ^ v = M - v for v <= M
        ll M = (1LL << B) - 1;
        return max_xor(x ^ M) ^ M;
    }
    ll count_less(ll x, ll k) const {
        if (k >> B) return size();
        int u = 0;
        ll r = 0;
        for (int b = B - 1; b >= 0; b--) {
            int c = x >> b & 1;
            if (k >> b & 1) {  // same bit as x -> xor bit 0 < 1: all count
                if (has(ch[u][c])) r += cnt[ch[u][c]];
                u = ch[u][c ^ 1];
            } else u = ch[u][c];
            if (!has(u)) return r;
        }
        return r;
    }
    ll kth(ll k) const {
        int u = 0;
        ll r = 0;
        for (int b = B - 1; b >= 0; b--) {
            int l = ch[u][0], lc = has(l) ? cnt[l] : 0;
            if (k < lc) u = l;
            else k -= lc, r |= 1LL << b, u = ch[u][1];
        }
        return r;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_xor_trie() {
    XorTrie t(4);
    for (ll x : {3, 10, 5, 5}) t.insert(x);
    cout << t.max_xor(6) << ' ' << t.min_xor(6) << ' '  // 12 3 5
         << t.kth(2) << '\n';
    t.erase(5);
    cout << t.count_less(1, 7) << ' ' << t.size() << '\n';  // 2 3
}
