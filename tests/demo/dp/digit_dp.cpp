#include "template/template.cpp"
#include "src/dp/digit_dp.cpp"

int main() {
    long long N;
    cin >> N;
    s = to_string(N);
    n = s.size();
    memset(dp, -1, sizeof(dp));
    cout << "Cantidad de números con suma de dígitos divisible por 3 en [0," << N << "] = "
         << solve(0, 0, true, false) << "\n";
}
