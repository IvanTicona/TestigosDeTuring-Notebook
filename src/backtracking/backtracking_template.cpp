#include <bits/stdc++.h>
using namespace std;

// Un estado parcial de la solución
vector<int> current;

// Para guardar todas las soluciones válidas (opcional)
vector<vector<int>> solutions;

// Función que verifica si un candidato es válido en el estado actual
bool isValid(int candidate) {
    // Aquí defines la condición de validez
    return true;
}

// Función recursiva de backtracking
void backtrack(int pos, int n) {
    if (pos == n) {
        // Caso base: alcanzamos una solución completa
        solutions.push_back(current);
        return;
    }

    // Generar candidatos para esta posición
    for (int cand = 1; cand <= n; cand++) {
        if (isValid(cand)) {
            current.push_back(cand);    // elegimos el candidato
            backtrack(pos + 1, n);      // exploramos la siguiente posición
            current.pop_back();         // backtrack (deshacemos la elección)
        }
    }
}

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
