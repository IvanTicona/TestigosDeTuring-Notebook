/**
 * Usage: DSU d(n); d.find(x), d.unite(a, b) (false if already joined),
 *          d.same(a, b), d.size(x), d.comps
 *        DSURollback r(n); r.unite(a, b); int t = r.time(); r.rollback(t)
 *          (undo every union after time t; no path compression)
 *        WeightedDSU w(n); w.unite(a, b, d) means pot[b] - pot[a] = d
 *          (false if it contradicts); w.diff(a, b) = pot[b] - pot[a]
 * Complexity: DSU ~O(1) amortized; rollback version O(log n) per find
 * Source: UMSS Disjoint Set Union (union by rank, recursive find) + UPB
 *         UnionFind (no rank) + UPB DSU_Rollback (rank undo guessed with
 *         r[a] == r[b] + 1: WRONG when the rank was already larger) + UMSS
 *         Dynamic Connectivity (saves both ranks); weighted: own
 * Note: parity / bipartite checks: WeightedDSU with d = 1 and potentials
 *       mod 2 (or xor). Kruskal: sort edges, unite. Rollback DSU is used by
 *       dynamic_connectivity.cpp (segment tree over time).
 * Status: untested
 */
struct DSU {
    vi p;  // p[x] < 0: x is a root of size -p[x]
    int comps;
    DSU(int n) : p(n, -1), comps(n) {}
    int find(int x) {
        while (p[x] >= 0 && p[p[x]] >= 0) p[x] = p[p[x]], x = p[x];  // halving
        return p[x] < 0 ? x : p[x];
    }
    bool same(int a, int b) { return find(a) == find(b); }
    int size(int x) { return -p[find(x)]; }
    bool unite(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return false;
        if (p[a] > p[b]) swap(a, b);
        p[a] += p[b], p[b] = a, comps--;
        return true;
    }
};

struct DSURollback {
    vi p;
    vector<array<int, 3>> st;  // (new root a, child b, old p[b])
    int comps;
    DSURollback(int n) : p(n, -1), comps(n) {}
    int find(int x) { while (p[x] >= 0) x = p[x]; return x; }
    int time() { return sz(st); }
    bool unite(int a, int b) {
        a = find(a), b = find(b);
        if (a == b) return false;
        if (p[a] > p[b]) swap(a, b);  // union by size: depth O(log n)
        st.pb({a, b, p[b]}), p[a] += p[b], p[b] = a, comps--;
        return true;
    }
    void rollback(int t) {
        while (sz(st) > t) {
            auto [a, b, old] = st.back();
            st.pop_back();
            p[a] -= old, p[b] = old, comps++;
        }
    }
};

struct WeightedDSU {
    vi p;
    vl pot;  // pot[x] - pot[parent]
    WeightedDSU(int n) : p(n, -1), pot(n, 0) {}
    int find(int x) {
        if (p[x] < 0) return x;
        int r = find(p[x]);
        pot[x] += pot[p[x]], p[x] = r;  // pot of the old parent is now final
        return r;
    }
    ll diff(int a, int b) { find(a), find(b); return pot[b] - pot[a]; }
    bool unite(int a, int b, ll d) {
        int ra = find(a), rb = find(b);
        if (ra == rb) return pot[b] - pot[a] == d;
        d += pot[a] - pot[b];  // now: pot[rb] - pot[ra] = d
        if (p[ra] > p[rb]) swap(ra, rb), d = -d;
        p[ra] += p[rb], p[rb] = ra, pot[rb] = d;
        return true;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_dsu() {
    DSU d(5);
    d.unite(0, 2), d.unite(1, 0), d.unite(3, 4);
    cout << d.same(1, 2) << d.same(2, 3) << ' '  // 10 3 2
         << d.size(2) << ' ' << d.comps << '\n';
    DSURollback r(4);
    r.unite(0, 1);
    int t = r.time();
    r.unite(1, 2), r.unite(2, 3);
    int before = r.comps;
    r.rollback(t);  // undo the last two unions
    bool j = r.find(3) == r.find(0);
    cout << before << ' ' << r.comps << ' ' << j << '\n';  // 1 3 0
    WeightedDSU w(3);  // x1 - x0 = 5, x2 - x1 = -2
    w.unite(0, 1, 5), w.unite(1, 2, -2);
    cout << w.diff(0, 2) << ' ' << w.unite(0, 2, 4) << '\n';  // 3 0
}
