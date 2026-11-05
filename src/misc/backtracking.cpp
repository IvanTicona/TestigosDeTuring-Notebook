/**
 * Usage: queens(n)       -> #ways to place n non-attacking queens (n <= 16)
 *        queens_one(n)   -> col[r] of one solution, {} if none
 * Complexity: exponential; bitmasks for columns and both diagonals make
 *             n = 14 instant (~1e8 nodes for n = 16)
 * Source: old backtracking_template + old n_queens (arrays, no masks)
 * Note: backtracking template: choose / recurse / undo; prune as early as
 *       possible (feasibility bounds, symmetry: count half and double).
 *       Branch and bound: keep the best answer and cut when the optimistic
 *       bound cannot beat it. Exact cover (sudoku): Dancing Links.
 * Status: untested
 */
ll queens_rec(int n, int row, int cols, int d1, int d2) {
    if (row == n) return 1;
    ll cnt = 0;
    int full = (1 << n) - 1;
    for (int free = full & ~(cols | d1 | d2); free; free &= free - 1) {
        int b = free & -free;  // try the lowest free column
        cnt += queens_rec(n, row + 1, cols | b, (d1 | b) << 1, (d2 | b) >> 1);
    }
    return cnt;
}
ll queens(int n) { return queens_rec(n, 0, 0, 0, 0); }

vi queens_one(int n) {
    vi col;
    auto rec = [&](auto &&self, int cols, int d1, int d2) -> bool {
        if (sz(col) == n) return true;
        int full = (1 << n) - 1;
        for (int free = full & ~(cols | d1 | d2); free; free &= free - 1) {
            int b = free & -free;
            col.pb(__builtin_ctz(b));
            if (self(self, cols | b, (d1 | b) << 1, (d2 | b) >> 1)) return true;
            col.pop_back();
        }
        return false;
    };
    return rec(rec, 0, 0, 0) ? col : vi();
}

// ---- Example: call from solve(); each // is the line it prints ----
void example_backtracking() {
    ll q4 = queens(4), q6 = queens(6), q8 = queens(8);
    cout << q4 << ' ' << q6 << ' ' << q8 << '\n';  // 2 4 92
    vi c = queens_one(4);  // row r has its queen in column c[r]
    int none = sz(queens_one(3));
    cout << c[0] << c[1] << c[2] << c[3] << ' ' << none << '\n';  // 1302 0
}
