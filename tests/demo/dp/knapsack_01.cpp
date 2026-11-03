#include "template/template.cpp"
#include "src/dp/knapsack_01.cpp"

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, W;
    cin >> n >> W;  // n = número de objetos, W = capacidad máxima

    vector<int> w(n+1), v(n+1);
    for (int i = 1; i <= n; i++) {
        cin >> w[i] >> v[i]; // peso y valor de cada objeto
    }

    cout << "Valor máximo = " << knapsack01(n, W, w, v) << "\n";
}
