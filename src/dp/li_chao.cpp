/**
 * Usage: LiChao t(lo, hi);  integer x in [lo, hi], MIN of lines
 *        t.add(k, m)            line k x + m on the whole range
 *        t.add_seg(k, m, l, r)  line only on x in [l, r]
 *        t.query(x)             -> min at x (INFL if no line covers x)
 * Complexity: add O(log C), add_seg O(log^2 C), query O(log C); nodes are
 *             created on demand (O(#adds * log C) memory)
 * Source: UPB LiChaoTree (raw new per node, half-open ranges) +
 *         cp-algorithms; for MAX negate k and m (and the answer)
 * Note: no precision issues and no sorted slopes needed (vs CHT). k x + m
 *       must fit in ll over [lo, hi]. Also works for any functions where
 *       two of them cross at most once (e.g. a (x - b)^2 + c with equal a).
 * Status: ok
 */
struct LiChao {
    struct Fn { ll k, m; ll operator()(ll x) const { return k * x + m; } };
    vector<Fn> t;
    vi lc, rc;
    ll lo, hi;
    LiChao(ll lo_, ll hi_) : lo(lo_), hi(hi_) { node(); }
    int node() { t.pb({0, INFL}), lc.pb(-1), rc.pb(-1); return sz(t) - 1; }
    void ins(Fn f, int v, ll l, ll r) {  // empty node = line {0, INFL}
        while (true) {
            ll md = l + (r - l) / 2;
            bool lef = f(l) < t[v](l), mid = f(md) < t[v](md);
            if (mid) swap(t[v], f);  // t[v] wins at md; f goes down
            if (l == r || f.m == INFL) return;
            if (lef != mid) {  // f better on part of [l, md)
                if (lc[v] < 0) { int c = node(); lc[v] = c; }
                v = lc[v], r = md;
            } else {           // f can only be better on (md, r]
                if (rc[v] < 0) { int c = node(); rc[v] = c; }
                v = rc[v], l = md + 1;
            }
        }
    }
    void add(ll k, ll m) { ins({k, m}, 0, lo, hi); }
    void seg(Fn f, ll ql, ll qr, int v, ll l, ll r) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { ins(f, v, l, r); return; }
        ll md = l + (r - l) / 2;
        if (lc[v] < 0) { int c = node(); lc[v] = c; }
        if (rc[v] < 0) { int c = node(); rc[v] = c; }
        seg(f, ql, qr, lc[v], l, md), seg(f, ql, qr, rc[v], md + 1, r);
    }
    void add_seg(ll k, ll m, ll l, ll r) { seg({k, m}, l, r, 0, lo, hi); }
    ll query(ll x) const {
        ll res = INFL, l = lo, r = hi;
        for (int v = 0; v >= 0;) {
            res = min(res, t[v](x));
            ll md = l + (r - l) / 2;
            if (x <= md) v = lc[v], r = md;
            else v = rc[v], l = md + 1;
        }
        return res;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_li_chao() {
    LiChao t(-100, 100);
    t.add(2, 0), t.add(-1, 12);  // min(2x, 12 - x)
    cout << t.query(1) << ' ' << t.query(10) << '\n';  // 2 2
    t.add_seg(0, -5, 3, 6);      // y = -5 only on [3, 6]
    cout << t.query(4) << ' ' << t.query(7) << '\n';   // -5 5
    LiChao e(0, 10);
    cout << (e.query(3) == INFL) << '\n';              // 1
}
