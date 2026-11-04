#include "template/template.cpp"
#include "src/dp/subset_sum_1d.cpp"

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, X;
    cin >> n >> X;

    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    if (subsetSum1d(X, a)) cout << "Yes, a subset exists with sum " << X << "\n";
    else cout << "No subset exists with sum " << X << "\n";
}
