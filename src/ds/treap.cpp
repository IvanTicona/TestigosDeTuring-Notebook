/**
 * Usage: Treap t;  implicit keys (positions), 0-indexed, inclusive ranges
 *        t.insert(pos, v)  (pos in [0, size]); t.erase(pos); t.get(pos)
 *        t.add(l, r, v); t.sum(l, r); t.reverse(l, r); t.size()
 *        t.split(t.root, k, a, b) / t.root = t.join(a, b): first k elements
 *        go to a -> cut / paste / rotate blocks; t.to_vec()
 * Complexity: O(log n) expected per operation
 * Source: UPB isc-notebook ds_treap (rotations; erase rotated the child
 *         with the LARGER priority up, breaking the heap) + UTP STL Treap;
 *         split / join with lazy reverse and add: own (classic)
 * Note: ordered set by key: split by value instead of size (or use oset
 *       from the template). Nodes live in vectors (node 0 = empty).
 *       Rope (__gnu_cxx::rope) is ~2x slower than this treap.
 * Status: untested
 */
struct Treap {
    vi L = {0}, R = {0}, cnt = {0};
    vector<ull> pr = {0};
    vl val = {0}, sm = {0}, lz = {0};
    vector<char> rv = {0};
    int root = 0;
    int node(ll v) {
        L.pb(0), R.pb(0), cnt.pb(1), pr.pb(rng()), val.pb(v), sm.pb(v);
        lz.pb(0), rv.pb(0);
        return sz(L) - 1;
    }
    void put(int t, ll a, bool r) {  // apply a pending add / reverse
        if (!t) return;
        val[t] += a, sm[t] += a * cnt[t], lz[t] += a;
        if (r) swap(L[t], R[t]), rv[t] ^= 1;
    }
    void push(int t) {
        put(L[t], lz[t], rv[t]), put(R[t], lz[t], rv[t]);
        lz[t] = 0, rv[t] = 0;
    }
    void pull(int t) {
        cnt[t] = cnt[L[t]] + cnt[R[t]] + 1;
        sm[t] = sm[L[t]] + sm[R[t]] + val[t];
    }
    void split(int t, int k, int &a, int &b) {  // a = first k elements
        if (!t) { a = b = 0; return; }
        push(t);
        if (cnt[L[t]] < k) split(R[t], k - cnt[L[t]] - 1, R[t], b), a = t;
        else split(L[t], k, a, L[t]), b = t;
        pull(t);
    }
    int join(int a, int b) {
        if (!a || !b) return a | b;
        if (pr[a] > pr[b]) { push(a), R[a] = join(R[a], b), pull(a); return a; }
        push(b), L[b] = join(a, L[b]), pull(b);
        return b;
    }
    int size() const { return cnt[root]; }
    void insert(int pos, ll v) {
        int a, b;
        split(root, pos, a, b);
        int nd = node(v);
        root = join(join(a, nd), b);
    }
    void erase(int pos) {
        int a, b, c;
        split(root, pos, a, b), split(b, 1, b, c);
        root = join(a, c);
    }
    template <class F>
    void range(int l, int r, F f) {  // f(middle tree root) on [l, r]
        int a, b, c;
        split(root, l, a, b), split(b, r - l + 1, b, c);
        f(b);
        root = join(join(a, b), c);
    }
    void add(int l, int r, ll v) { range(l, r, [&](int t) { put(t, v, 0); }); }
    void reverse(int l, int r) { range(l, r, [&](int t) { put(t, 0, 1); }); }
    ll sum(int l, int r) {
        ll s = 0;
        range(l, r, [&](int t) { s = sm[t]; });
        return s;
    }
    ll get(int pos) { return sum(pos, pos); }
    vl to_vec() {
        vl out;
        auto dfs = [&](auto &&self, int t) -> void {
            if (!t) return;
            push(t), self(self, L[t]), out.pb(val[t]), self(self, R[t]);
        };
        dfs(dfs, root);
        return out;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_treap() {
    Treap t;
    for (int i = 0; i < 6; i++) t.insert(i, i + 1);  // 1 2 3 4 5 6
    t.reverse(1, 4);   // 1 5 4 3 2 6
    t.add(0, 2, 10);   // 11 15 14 3 2 6
    t.erase(3);        // 11 15 14 2 6
    vl v = t.to_vec();
    cout << v[0] << ' ' << v[1] << ' ' << v[2] << ' '  // 11 15 14 2 6
         << v[3] << ' ' << v[4] << '\n';
    ll s = t.sum(1, 3);  // 15 + 14 + 2
    cout << s << ' ' << t.get(4) << ' ' << t.size() << '\n';  // 31 6 5
}
