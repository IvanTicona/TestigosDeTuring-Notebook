#include "template/template.cpp"
#include "src/backtracking/n_queens.cpp"

int main() {
    cin >> N; // tamaño del tablero
    col.resize(N);
    solve(0);
    cout << "Número total de soluciones = " << solutions << "\n";
}
