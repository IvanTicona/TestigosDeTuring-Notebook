#include "template/template.cpp"
#include "src/dp/edit_distance.cpp"

int main() {
    string s1, s2;
    cout << "Introduce el primer string: ";
    cin >> s1;
    cout << "Introduce el segundo string: ";
    cin >> s2;

    cout << "La distancia de edición es: " << editDistance(s1, s2) << endl;
    return 0;
}
