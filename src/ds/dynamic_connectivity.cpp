/**
 * Usage: DynCon dc(n);  events in time order (offline):
 *        dc.add(u, v), dc.remove(u, v) (edge must exist; multi-edges ok),
 *        dc.ask_comps() (#components), dc.ask(u, v) (connected?)
 *        vl ans = dc.solve();  -> answers of the asks, in order
 * Complexity: O(T log T log n), T = #events
 * Source: UMSS Dynamic Connectivity (segment tree over time + rollback
 *         DSU); multi-edges and connectivity queries: own
 * Note: an edge alive in [s, e) is added to O(log T) segment-tree nodes;
 *       DFS over the tree uniting on entry, rolling back on exit. Same
 *       trick for any "add-only + undo" structure (e.g. bipartiteness
 *       with a parity DSU, xor basis copies).
 * Depends: src/ds/dsu.cpp
 * Status: untested
 */
struct DynCon {
    int n;
    vector<array<int, 3>> ev;  // {type, u, v}: 0 add, 1 remove, 2 comps, 3 ask
    DynCon(int n_) : n(n_) {}
    void add(int u, int v) { ev.pb({0, min(u, v), max(u, v)}); }
    void remove(int u, int v) { ev.pb({1, min(u, v), max(u, v)}); }
    void ask_comps() { ev.pb({2, 0, 0}); }
    void ask(int u, int v) { ev.pb({3, u, v}); }
    vector<vector<pii>> seg;
    void ins(int x, int l, int r, int ql, int qr, pii e) {  // alive [ql, qr]
        if (qr < l || r < ql) return;
        if (ql <= l && r <= qr) { seg[x].pb(e); return; }
        int m = (l + r) / 2;
        ins(2 * x, l, m, ql, qr, e), ins(2 * x + 1, m + 1, r, ql, qr, e);
    }
    vl solve() {
        int T = sz(ev);
        if (!T) return {};
        seg.assign(4 * T, {});
        map<pii, vi> open;  // edge -> start times (stack for multi-edges)
        for (int t = 0; t < T; t++) {
            auto [ty, u, v] = ev[t];
            if (ty == 0) open[{u, v}].pb(t);
            if (ty == 1) {
                vi &st = open[{u, v}];
                ins(1, 0, T - 1, st.back(), t - 1, {u, v}), st.pop_back();
            }
        }
        for (auto &[e, st] : open)
            for (int s : st) ins(1, 0, T - 1, s, T - 1, e);
        DSURollback d(n);
        vl ans;
        auto dfs = [&](auto &&self, int x, int l, int r) -> void {
            int tm = d.time();
            for (auto [u, v] : seg[x]) d.unite(u, v);
            if (l == r) {
                if (ev[l][0] == 2) ans.pb(d.comps);
                if (ev[l][0] == 3)
                    ans.pb(d.find(ev[l][1]) == d.find(ev[l][2]));
            } else {
                int m = (l + r) / 2;
                self(self, 2 * x, l, m), self(self, 2 * x + 1, m + 1, r);
            }
            d.rollback(tm);
        };
        dfs(dfs, 1, 0, T - 1);
        return ans;
    }
};

// ---- Example: call from solve(); each // is the line it prints ----
void example_dynamic_connectivity() {
    DynCon dc(4);
    dc.add(0, 1), dc.add(1, 2), dc.ask_comps(), dc.ask(0, 2);
    dc.remove(1, 2), dc.ask_comps(), dc.ask(0, 2);
    vl a = dc.solve();
    cout << a[0] << a[1] << ' ' << a[2] << a[3] << '\n';  // 21 30
}
