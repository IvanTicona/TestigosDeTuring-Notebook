/**
 * Uso: computeZ(s) -> Z[i] = longitud del mayor prefijo de s que empieza en i; para buscar patron: computeZ(pat + "$" + text)
 * Complejidad: O(n)
 * Status: untested
 */
// Función que calcula el array Z de una cadena
vector<int> computeZ(const string &s) {
    int n = s.size();
    vector<int> Z(n, 0);
    int l = 0, r = 0; // [l, r] es la ventana de coincidencia más a la derecha

    for (int i = 1; i < n; i++) {
        if (i <= r)
            Z[i] = min(r - i + 1, Z[i - l]);

        while (i + Z[i] < n && s[Z[i]] == s[i + Z[i]])
            Z[i]++;

        if (i + Z[i] - 1 > r) {
            l = i;
            r = i + Z[i] - 1;
        }
    }
    return Z;
}
