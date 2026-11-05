// Stress test for src/flows/general_matching.cpp
// Oracle: bitmask DP over vertex subsets (n <= 14): best[mask] = max
// matching using only vertices of mask (take the lowest vertex unmatched
// or matched to a neighbor); dense random graphs with many odd cycles.
// The returned matching must be symmetric and use only existing edges.
#include "tests/stress/template_lib.h"
#include "src/flows/general_matching.cpp"
#include "tests/stress/common.h"

int main() {
    stress::check_example(example_general_matching, "src/flows/general_matching.cpp", "example_general_matching");
    for (stress::iter = 0; stress::iter < stress::scaled(1500); stress::iter++) {
        int n = stress::rnd(1, 14);
        double p = stress::rnd(5, 60) / 100.0;
        vector<pii> E;
        vector<vector<char>> adj(n, vector<char>(n, 0));
        for (int u = 0; u < n; u++)
            for (int v = u + 1; v < n; v++)
                if (stress::rnd(0, 99) < p * 100) E.pb({u, v}), adj[u][v] = adj[v][u] = 1;
        if (stress::rnd(0, 3) == 0 && n > 0) E.pb({0, 0});  // self-loop ignored
        vi best(1 << n, 0);
        for (int mask = 1; mask < (1 << n); mask++) {
            int v = __builtin_ctz(mask), rest = mask ^ (1 << v);
            best[mask] = best[rest];
            for (int u = 0; u < n; u++)
                if ((rest >> u & 1) && adj[v][u]) best[mask] = max(best[mask], 1 + best[rest ^ (1 << u)]);
        }
        vi mt = general_matching(n, E);
        int k = 0;
        for (int v = 0; v < n; v++)
            if (mt[v] >= 0) {
                k++;
                CHECK(mt[mt[v]] == v && adj[v][mt[v]], "valid matching");
            }
        CHECK(k / 2 == best[(1 << n) - 1], "size n=" + to_string(n));
    }
    puts("OK");
}
