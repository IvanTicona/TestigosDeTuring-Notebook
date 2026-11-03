#include "template/template.cpp"
#include "src/dp/subset_sum.cpp"

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, X;
    cin >> n >> X; // number of elements and target sum

    vector<int> a(n);
    for (int i = 0; i < n; i++) cin >> a[i];

    if (subsetSum(n, X, a)) cout << "Yes, a subset exists with sum " << X << "\n";
    else cout << "No subset exists with sum " << X << "\n";
}
