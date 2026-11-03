#include "template/template.cpp"
#include "src/backtracking/backtracking_template.cpp"

int main() {
    int n;
    cin >> n; // problem size

    backtrack(0, n);

    cout << "Number of solutions = " << solutions.size() << "\n";
    for (auto &sol : solutions) {
        for (int x : sol) cout << x << " ";
        cout << "\n";
    }
}
