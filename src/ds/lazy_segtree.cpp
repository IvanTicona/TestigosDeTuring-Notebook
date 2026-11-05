/**
 * Usage: LazySeg st(a);  (0-indexed, inclusive ranges, a non-empty)
 *        st.update(l, r, tag)   e.g. Tag{true, 5, 0} = assign 5,
 *                               Tag{false, 0, 3} = add 3
 *        st.query(l, r)         -> Node {sum, mn, len} of a[l..r]
 * Complexity: O(log n) per operation, 4n nodes
 * Source: UMSS General Lazy Tree (recursive; push wrote the children's
 *         lazy through a stale copy) + UMSS Iterative Lazytree (SET / ADD
 *         composition, CSES 1735) + UPB SegmentTreeLazy (flip only)
 * Note: EDIT Node / Tag / combine / apply_tag / compose for other problems.
 *       Law: apply_tag(apply_tag(x, f), g) == apply_tag(x, compose(f, g)).
 *       Affine maps x -> b x + c (sum needs len): compose = (b1 b2,
 *       c1 b2 + c2). Range flip of bits: Tag = xor flag, sum = len - sum.
 * Status: untested
 */
struct Node { ll sum = 0, mn = INFL, len = 0; };
struct Tag { bool set = false; ll val = 0, add = 0; };  // set first, then add
Node combine(const Node &a, const Node &b) {
    return {a.sum + b.sum, min(a.mn, b.mn), a.len + b.len};
}
Node apply_tag(Node x, const Tag &g) {
    if (g.set) x.sum = g.val * x.len, x.mn = g.val;
    x.sum += g.add * x.len, x.mn += g.add;
    return x;
}
Tag compose(const Tag &f, const Tag &g) {  // f first, then g
    if (g.set) return g;
    return {f.set, f.val, f.add + g.add};
}

struct LazySeg {
    int n;
    vector<Node> t;
    vector<Tag> lz;
    LazySeg(const vl &a) : n(sz(a)), t(4 * n), lz(4 * n) {
        build(1, 0, n - 1, a);
    }
    void build(int v, int l, int r, const vl &a) {
        if (l == r) { t[v] = {a[l], a[l], 1}; return; }
        int m = (l + r) / 2;
        build(2 * v, l, m, a), build(2 * v + 1, m + 1, r, a);
        t[v] = combine(t[2 * v], t[2 * v + 1]);
    }
    void put(int v, const Tag &g) {
        t[v] = apply_tag(t[v], g), lz[v] = compose(lz[v], g);
    }
    void push(int v) {
        put(2 * v, lz[v]), put(2 * v + 1, lz[v]), lz[v] = Tag();
    }
    void update(int ql, int qr, const Tag &g, int v, int l, int r) {
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { put(v, g); return; }
        push(v);
        int m = (l + r) / 2;
        update(ql, qr, g, 2 * v, l, m), update(ql, qr, g, 2 * v + 1, m + 1, r);
        t[v] = combine(t[2 * v], t[2 * v + 1]);
    }
    Node query(int ql, int qr, int v, int l, int r) {
        if (qr < l || r < ql) return Node();
        if (ql <= l && r <= qr) return t[v];
        push(v);
        int m = (l + r) / 2;
        return combine(query(ql, qr, 2 * v, l, m),
                       query(ql, qr, 2 * v + 1, m + 1, r));
    }
    void update(int l, int r, const Tag &g) { update(l, r, g, 1, 0, n - 1); }
    Node query(int l, int r) { return query(l, r, 1, 0, n - 1); }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_lazy_segtree() {
    LazySeg st(vl{1, 2, 3, 4, 5});
    st.update(1, 3, Tag{false, 0, 10});  // 1 12 13 14 5
    st.update(2, 4, Tag{true, 7, 0});    // 1 12 7 7 7
    Node q = st.query(0, 4);
    cout << q.sum << ' ' << q.mn << ' '  // 34 1 19
         << st.query(1, 2).sum << '\n';
}
