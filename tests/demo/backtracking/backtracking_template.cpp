#include "template/template.cpp"
#include "src/backtracking/backtracking_template.cpp"

int main() {
    int n;
    cin >> n; // tamaño del problema

    backtrack(0, n);

    cout << "Número de soluciones = " << solutions.size() << "\n";
    for (auto &sol : solutions) {
        for (int x : sol) cout << x << " ";
        cout << "\n";
    }
}
