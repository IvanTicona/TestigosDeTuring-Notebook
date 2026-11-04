#include "template/template.cpp"
#include "src/dp/digit_dp.cpp"

int main() {
    long long N;
    cin >> N;
    s = to_string(N);
    n = s.size();
    memset(dp, -1, sizeof(dp));
    cout << "Count of numbers with digit sum divisible by 3 in [0," << N << "] = "
         << solve(0, 0, true, false) << "\n";
}
