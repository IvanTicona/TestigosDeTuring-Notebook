#include "template/template.cpp"
#include "src/dp/knapsack_01_1d.cpp"

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, W;
    cin >> n >> W;

    vector<int> w(n), v(n);
    for (int i = 0; i < n; i++) cin >> w[i] >> v[i];

    cout << "Valor máximo = " << knapsack01_1d(n, W, w, v) << "\n";
}
