#include "template/template.cpp"
#include "src/dp/lcs.cpp"

int main() {
    string X = "AGGTAB";
    string Y = "GXTXAYB";

    cout << "Length of LCS: " << LCS(X, Y) << "\n";
    cout << "LCS: " << getLCS(X, Y) << "\n";

    return 0;
}
