#include "template/template.cpp"
#include "src/dp/edit_distance.cpp"

int main() {
    string s1, s2;
    cout << "Enter the first string: ";
    cin >> s1;
    cout << "Enter the second string: ";
    cin >> s2;

    cout << "The edit distance is: " << editDistance(s1, s2) << endl;
    return 0;
}
