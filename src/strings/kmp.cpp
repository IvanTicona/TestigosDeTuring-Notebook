/**
 * Uso: KMP(text, pat) -> posiciones (0-indexadas) de todas las ocurrencias de pat en text; computeLPS(pat) arreglo de fallos
 * Complejidad: O(n + m)
 * Status: untested
 */
// Construir arreglo LPS (Longest Prefix Suffix)
vector<int> computeLPS(const string &pat) {
    int m = pat.size();
    vector<int> lps(m, 0);
    int len = 0;
    int i = 1;
    while (i < m) {
        if (pat[i] == pat[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

// Función KMP
vector<int> KMP(const string &text, const string &pat) {
    vector<int> lps = computeLPS(pat);
    vector<int> positions;
    int n = text.size(), m = pat.size();
    int i = 0, j = 0;
    while (i < n) {
        if (text[i] == pat[j]) {
            i++; j++;
        }
        if (j == m) {
            positions.push_back(i - j);
            j = lps[j - 1];
        } else if (i < n && text[i] != pat[j]) {
            if (j != 0) j = lps[j - 1];
            else i++;
        }
    }
    return positions;
}
