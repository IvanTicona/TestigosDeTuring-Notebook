/**
 * Usage: cin >> N; col.resize(N); solve(0); prints each board and counts them in solutions
 * Complexity: O(N!) approximate (pruning by column and diagonal)
 * Status: untested
 */
int N;
vector<int> col;   // col[i] = column of the queen in row i
int solutions = 0;

// Checks whether it is safe to place a queen at row=row, col=c
bool safe(int row, int c) {
    for (int r = 0; r < row; r++) {
        int c2 = col[r];
        if (c2 == c) return false;                   // same column
        if (abs(c2 - c) == abs(r - row)) return false; // same diagonal
    }
    return true;
}

void solve(int row) {
    if (row == N) {
        solutions++;
        // If you want to print a solution:
        cout << "Solution " << solutions << ":\n";
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                cout << (col[r] == c ? "Q " : ". ");
            }
            cout << "\n";
        }
        cout << "\n";
        return;
    }

    for (int c = 0; c < N; c++) {
        if (safe(row, c)) {
            col[row] = c;
            solve(row + 1);
        }
    }
}
