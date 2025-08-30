#include <bits/stdc++.h>
using namespace std;

int N;
vector<int> col;   // col[i] = columna en la que está la reina en la fila i
int solutions = 0;

// Verifica si es seguro colocar reina en fila=row, col=c
bool safe(int row, int c) {
    for (int r = 0; r < row; r++) {
        int c2 = col[r];
        if (c2 == c) return false;                   // misma columna
        if (abs(c2 - c) == abs(r - row)) return false; // misma diagonal
    }
    return true;
}

void solve(int row) {
    if (row == N) {
        solutions++;
        // Si quieres imprimir una solución:
        cout << "Solución " << solutions << ":\n";
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

int main() {
    cin >> N; // tamaño del tablero
    col.resize(N);
    solve(0);
    cout << "Número total de soluciones = " << solutions << "\n";
}
