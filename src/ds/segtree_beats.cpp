/**
 * Usage: Beats b(a);  0-indexed, inclusive ranges, a non-empty,
 *        n * max|value| < ~9e18 (sums are ll)
 *        b.chmin(l, r, x)  a[i] = min(a[i], x);  b.chmax(l, r, x)
 *        b.add(l, r, x);   b.sum(l, r), b.qmax(l, r), b.qmin(l, r)
 * Complexity: O(log^2 n) amortized per update (O(log n) with only chmin /
 *             chmax and no add), O(log n) per query
 * Source: own (Ji's "segment tree beats", Gorgeous Sequence); missing in
 *         all three references
 * Note: a node keeps max, second max (strict), count of max, and the same
 *       for min. chmin only touches nodes where 2nd max < x < max (then
 *       only the max values change). Other "beats" ops: range mod, range
 *       sqrt (stop when max == min), range gcd with point updates.
 * Status: ok
 */
struct Beats {
    int n;
    vl mx1, mx2, cmx, mn1, mn2, cmn, sm, lz;
    Beats(const vl &a) : n(sz(a)) {
        for (auto *v : {&mx1, &mx2, &cmx, &mn1, &mn2, &cmn, &sm, &lz})
            v->assign(4 * n, 0);
        build(1, 0, n - 1, a);
    }
    void pull(int v) {
        int l = 2 * v, r = 2 * v + 1;
        sm[v] = sm[l] + sm[r];
        if (mx1[l] < mx1[r]) swap(l, r);  // l has the larger max
        bool eq = mx1[l] == mx1[r];
        mx1[v] = mx1[l], cmx[v] = cmx[l] + (eq ? cmx[r] : 0);
        mx2[v] = std::max(mx2[l], eq ? mx2[r] : mx1[r]);
        l = 2 * v, r = 2 * v + 1;
        if (mn1[l] > mn1[r]) swap(l, r);  // l has the smaller min
        eq = mn1[l] == mn1[r];
        mn1[v] = mn1[l], cmn[v] = cmn[l] + (eq ? cmn[r] : 0);
        mn2[v] = std::min(mn2[l], eq ? mn2[r] : mn1[r]);
    }
    void build(int v, int l, int r, const vl &a) {
        if (l == r) {
            mx1[v] = mn1[v] = sm[v] = a[l], cmx[v] = cmn[v] = 1;
            mx2[v] = -INFL, mn2[v] = INFL;
            return;
        }
        int m = (l + r) / 2;
        build(2 * v, l, m, a), build(2 * v + 1, m + 1, r, a), pull(v);
    }
    void put_add(int v, int len, ll x) {
        mx1[v] += x, mn1[v] += x, sm[v] += x * len, lz[v] += x;
        if (mx2[v] != -INFL) mx2[v] += x;
        if (mn2[v] != INFL) mn2[v] += x;
    }
    void put_chmin(int v, ll x) {  // requires mx2 < x < mx1
        if (x >= mx1[v]) return;
        sm[v] -= (mx1[v] - x) * cmx[v];
        if (mn1[v] == mx1[v]) mn1[v] = x;
        else if (mn2[v] == mx1[v]) mn2[v] = x;
        mx1[v] = x;
    }
    void put_chmax(int v, ll x) {  // requires mn1 < x < mn2
        if (x <= mn1[v]) return;
        sm[v] += (x - mn1[v]) * cmn[v];
        if (mx1[v] == mn1[v]) mx1[v] = x;
        else if (mx2[v] == mn1[v]) mx2[v] = x;
        mn1[v] = x;
    }
    void push(int v, int l, int r) {
        int m = (l + r) / 2;
        for (int c : {2 * v, 2 * v + 1}) {
            if (lz[v]) put_add(c, c == 2 * v ? m - l + 1 : r - m, lz[v]);
            put_chmin(c, mx1[v]), put_chmax(c, mn1[v]);
        }
        lz[v] = 0;
    }
    // ty: 0 chmin, 1 chmax, 2 add
    void upd(int ty, int ql, int qr, ll x, int v, int l, int r) {
        if (qr < l || r < ql) return;
        if (ty == 0 && mx1[v] <= x) return;
        if (ty == 1 && mn1[v] >= x) return;
        if (ql <= l && r <= qr) {
            if (ty == 2) { put_add(v, r - l + 1, x); return; }
            if (ty == 0 && mx2[v] < x) { put_chmin(v, x); return; }
            if (ty == 1 && mn2[v] > x) return put_chmax(v, x);
        }
        push(v, l, r);
        int m = (l + r) / 2;
        upd(ty, ql, qr, x, 2 * v, l, m);
        upd(ty, ql, qr, x, 2 * v + 1, m + 1, r);
        pull(v);
    }
    // what: 0 sum, 1 max, 2 min
    ll qry(int what, int ql, int qr, int v, int l, int r) {
        if (qr < l || r < ql) return what == 0 ? 0 : what == 1 ? -INFL : INFL;
        if (ql <= l && r <= qr)
            return what == 0 ? sm[v] : what == 1 ? mx1[v] : mn1[v];
        push(v, l, r);
        int m = (l + r) / 2;
        ll a = qry(what, ql, qr, 2 * v, l, m);
        ll b = qry(what, ql, qr, 2 * v + 1, m + 1, r);
        return what == 0 ? a + b : what == 1 ? std::max(a, b) : std::min(a, b);
    }
    void chmin(int l, int r, ll x) { upd(0, l, r, x, 1, 0, n - 1); }
    void chmax(int l, int r, ll x) { upd(1, l, r, x, 1, 0, n - 1); }
    void add(int l, int r, ll x) { upd(2, l, r, x, 1, 0, n - 1); }
    ll sum(int l, int r) { return qry(0, l, r, 1, 0, n - 1); }
    ll qmax(int l, int r) { return qry(1, l, r, 1, 0, n - 1); }
    ll qmin(int l, int r) { return qry(2, l, r, 1, 0, n - 1); }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_segtree_beats() {
    Beats b(vl{5, 1, 8, 3, 6});
    b.chmin(0, 4, 4);  // 4 1 4 3 4
    b.chmax(1, 3, 3);  // 4 3 4 3 4
    b.add(2, 4, 2);    // 4 3 6 5 6
    cout << b.sum(0, 4) << ' ' << b.qmax(0, 1) << ' '  // 24 4 5
         << b.qmin(2, 4) << '\n';
}
