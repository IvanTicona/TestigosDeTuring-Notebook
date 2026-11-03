#include "template/template.cpp"
#include "src/dp/subset_sum.cpp"

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, X;
    cin >> n >> X; // número de elementos y suma objetivo

    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    if (subsetSum(n, X, a)) cout << "Sí, existe un subconjunto con suma " << X << "\n";
    else cout << "No existe subconjunto con suma " << X << "\n";
}
