#include "template/template.cpp"
#include "src/dp/knapsack_01.cpp"

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, W;
    cin >> n >> W;  // n = number of items, W = maximum capacity

    vector<int> w(n+1), v(n+1);
    for (int i = 1; i <= n; i++) {
        cin >> w[i] >> v[i]; // weight and value of each item
    }

    cout << "Maximum value = " << knapsack01(n, W, w, v) << "\n";
}
