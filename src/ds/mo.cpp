/**
 * Usage: vl res = mo(n, qs, add, rem, get, hilbert = true);
 *        qs[i] = {l, r} (inclusive, 0-indexed); add(i) / rem(i) put / take
 *        a[i] into / out of the window; get() = answer of the window
 * Complexity: O((n + q) sqrt n) add/rem calls (Hilbert: often ~2x fewer)
 * Source: UMSS Mo's + UMSS Mo's Hilbert Curve (gilbertOrder) + UPB
 *         MosAlgorithm (zig-zag blocks, fixed distinct-count example) + UTP
 *         Mo's; generic lambdas: own
 * Note: add / rem must be O(1) (or O(log)). Order of moves: grow first,
 *       then shrink, so the window is never "negative". Mo on trees: run
 *       on the Euler tour (trees section). With updates: add a time
 *       dimension, blocks of n^(2/3).
 * Status: ok
 */
ll hilbert(int x, int y, int pw = 21, int rot = 0) {  // 2^pw > n
    if (pw == 0) return 0;
    int h = 1 << (pw - 1), seg = x < h ? (y < h ? 0 : 3) : (y < h ? 1 : 2);
    seg = (seg + rot) & 3;
    const int d[4] = {3, 0, 0, 1};
    int nx = x & (x ^ h), ny = y & (y ^ h), nrot = (rot + d[seg]) & 3;
    ll sub = 1LL << (2 * pw - 2), a = hilbert(nx, ny, pw - 1, nrot);
    ll r = seg * sub;
    return r + (seg == 1 || seg == 2 ? a : sub - a - 1);
}

template <class Add, class Rem, class Get>
vl mo(int n, const vector<pii> &qs, Add add, Rem rem, Get get,
      bool hil = true) {
    int q = sz(qs), B = max(1, (int)(n / sqrt(max(q, 1))));
    vi ord(q);
    iota(all(ord), 0);
    vl key(q);
    for (int i = 0; i < q; i++) {
        auto [l, r] = qs[i];
        ll blk = l / B;  // zig-zag: odd blocks sort r descending
        key[i] = hil ? hilbert(l, r) : blk * (n + 1) + (blk & 1 ? n - r : r);
    }
    sort(all(ord), [&](int a, int b) { return key[a] < key[b]; });
    vl res(q);
    int L = 0, R = -1;
    for (int i : ord) {
        auto [l, r] = qs[i];
        while (R < r) add(++R);
        while (L > l) add(--L);
        while (R > r) rem(R--);
        while (L < l) rem(L++);
        res[i] = get();
    }
    return res;
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_mo() {
    vi a = {1, 2, 1, 3, 2, 2};
    vi cnt(4, 0);
    int distinct = 0;
    auto add = [&](int i) { distinct += cnt[a[i]]++ == 0; };
    auto rem = [&](int i) { distinct -= --cnt[a[i]] == 0; };
    auto get = [&] { return distinct; };
    vl r = mo(6, {{0, 2}, {1, 4}, {4, 5}, {0, 5}}, add, rem, get);
    cout << r[0] << ' ' << r[1] << ' ' << r[2] << ' ' << r[3]  // 2 3 1 3
         << '\n';
}
