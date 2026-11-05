/**
 * Usage: auto [ver, es] = virtual_tree(et, keys);  et = EulerTour of the tree
 *        ver = keys + all their pairwise LCAs (sorted by tin, ver[0] = root
 *        of the virtual tree); es = edges {parent, child} where parent is
 *        the nearest ancestor of child inside ver (dist = depth difference)
 * Complexity: O(k log k) for k keys (O(1) LCA)
 * Source: UPB isc misc_virtual_tree (globals, 1e6 arrays, problem code
 *         mixed in); clean version: own
 * Note: |ver| <= 2k - 1. Run a DP on the virtual tree per query: total
 *       O(sum k log k) instead of O(n q). Edge weights of the compressed
 *       tree: dep[child] - dep[parent] (or weighted depths).
 * Depends: src/trees/euler_tour.cpp
 * Status: ok
 */
pair<vi, vector<pii>> virtual_tree(const EulerTour &et, vi keys) {
    auto by_tin = [&](int a, int b) { return et.tin[a] < et.tin[b]; };
    sort(all(keys), by_tin);
    int k = sz(keys);
    for (int i = 0; i + 1 < k; i++) keys.pb(et.lca(keys[i], keys[i + 1]));
    sort(all(keys), by_tin);
    keys.erase(unique(all(keys)), keys.end());
    vector<pii> es;
    for (int i = 1; i < sz(keys); i++)  // parent = lca with the previous one
        es.pb({et.lca(keys[i - 1], keys[i]), keys[i]});
    return {keys, es};
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_virtual_tree() {
    // 0 -> 1 -> {3, 4}, 0 -> 2 -> 5
    vector<vi> g = {{1, 2}, {0, 3, 4}, {0, 5}, {1}, {1}, {2}};
    EulerTour et(g, 0);
    auto [ver, es] = virtual_tree(et, {3, 4, 5});
    cout << sz(ver) << ' ' << ver[0] << ' ' << sz(es) << '\n';  // 5 0 4
}
