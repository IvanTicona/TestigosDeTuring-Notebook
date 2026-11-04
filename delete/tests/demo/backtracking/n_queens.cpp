#include "template/template.cpp"
#include "src/backtracking/n_queens.cpp"

int main() {
    cin >> N; // board size
    col.resize(N);
    solve(0);
    cout << "Total number of solutions = " << solutions << "\n";
}
