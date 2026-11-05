/**
 * Usage: LinkCut t(n);  dynamic forest on vertices 0..n-1, value per vertex
 *        t.link(u, v) (false if already connected), t.cut(u, v) (false if
 *        no such edge), t.connected(u, v), t.set(v, x),
 *        t.path_sum(u, v) (sum of values on the u - v path, connected),
 *        t.lca(r, u, v) (LCA when the tree is rooted at r)
 * Complexity: O(log n) amortized per operation (splay trees)
 * Source: UPB LinkCutTree + UPB isc ds_link_cut_tree (connected() via
 *         access(v) != -1 is wrong across components); own clean version
 * Note: path aggregate = any commutative op in pull (sum / min / max);
 *       non-commutative ops must also keep the reversed aggregate. Edge
 *       values: add a vertex per edge. Subtree aggregates need "virtual
 *       children" sums (not here).
 * Status: ok
 */
struct LinkCut {
    vector<array<int, 2>> c;  // nodes 1..n (0 = null)
    vi p;
    vector<char> rev;
    vl val, sm;
    LinkCut(int n)
        : c(n + 1, {0, 0}), p(n + 1, 0), rev(n + 1, 0), val(n + 1), sm(n + 1) {}
    bool isroot(int x) {
        return !p[x] || (c[p[x]][0] != x && c[p[x]][1] != x);
    }
    void push(int x) {
        if (!rev[x]) return;
        swap(c[x][0], c[x][1]);
        for (int y : c[x]) if (y) rev[y] ^= 1;
        rev[x] = 0;
    }
    void pull(int x) { sm[x] = sm[c[x][0]] + val[x] + sm[c[x][1]]; }
    void rot(int x) {
        int y = p[x], z = p[y], d = c[y][1] == x;
        if (!isroot(y)) c[z][c[z][1] == y] = x;
        p[x] = z, c[y][d] = c[x][d ^ 1];
        if (c[x][d ^ 1]) p[c[x][d ^ 1]] = y;
        c[x][d ^ 1] = y, p[y] = x;
        pull(y), pull(x);
    }
    void splay(int x) {
        vi st = {x};
        for (int y = x; !isroot(y); y = p[y]) st.pb(p[y]);
        while (!st.empty()) push(st.back()), st.pop_back();
        while (!isroot(x)) {
            int y = p[x], z = p[y];
            if (!isroot(y)) rot((c[y][1] == x) == (c[z][1] == y) ? y : x);
            rot(x);
        }
    }
    int access(int x) {  // returns the last node where the path joined
        int last = 0;
        for (int y = x; y; y = p[y])
            splay(y), c[y][1] = last, pull(y), last = y;
        splay(x);
        return last;
    }
    void make_root(int x) { access(x), rev[x] ^= 1, push(x); }
    int find_root(int x) {
        access(x);
        while (push(x), c[x][0]) x = c[x][0];
        splay(x);
        return x;
    }
    bool connected(int u, int v) {
        return find_root(u + 1) == find_root(v + 1);
    }
    bool link(int u, int v) {
        if (connected(u, v)) return false;
        make_root(u + 1), p[u + 1] = v + 1;
        return true;
    }
    bool cut(int u, int v) {
        int a = u + 1, b = v + 1;
        make_root(a), access(b);
        if (c[b][0] != a || c[a][1]) return false;  // a is b's direct child
        c[b][0] = p[a] = 0, pull(b);
        return true;
    }
    void set(int v, ll x) { access(v + 1), val[v + 1] = x, pull(v + 1); }
    ll path_sum(int u, int v) {
        make_root(u + 1), access(v + 1);
        return sm[v + 1];
    }
    int lca(int r, int u, int v) {
        make_root(r + 1), access(u + 1);
        return access(v + 1) - 1;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_link_cut() {
    LinkCut t(5);
    for (int v = 0; v < 5; v++) t.set(v, v + 1);  // values 1..5
    t.link(0, 1), t.link(1, 2), t.link(1, 3), t.link(3, 4);
    // path 2-1-3-4 = 3 + 2 + 4 + 5; linking 2-4 would close a cycle
    cout << t.path_sum(2, 4) << ' ' << t.lca(0, 2, 4) << ' '  // 14 1 0
         << t.link(2, 4) << '\n';
    t.cut(1, 3);
    cout << t.connected(2, 4) << ' ' << t.path_sum(3, 4) << '\n';  // 0 9
}
