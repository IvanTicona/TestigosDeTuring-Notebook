# Testigos de Turing - ICPC Team Notebook

Notebook de equipo para ICPC (equipo "Testigos de Turing", Universidad Privada Boliviana).
Los algoritmos viven como snippets de C++ y un script en Python los valida y arma un PDF
en LaTeX (carta apaisado, 2 columnas), en dos temas: lectura (color) e impresion (blanco y negro).

## Estructura

```
contents.txt        manifiesto: unica fuente de orden y titulos del PDF
template/           template.cpp: includes, typedefs y macros compartidos (sin main)
src/<tema>/*.cpp    snippets de libreria (solo funciones/clases, sin includes ni main)
tests/demo/<tema>/  demos con main() que incluyen template + snippet
tex/notebook.tex    documento LaTeX (portada, indice, cabecera, estilo de codigo)
tex/assets/         logo; tex/theory/*.tex: teoria opcional (!tex)
scripts/notebook.py CLI: check | tex | pdf
build/              salida generada (ignorada por git)
```

## Requisitos

- Python 3 (solo libreria estandar).
- `g++` con soporte `-std=gnu++17` (MinGW en Windows).
- `latexmk` y una distribucion LaTeX: MiKTeX (Windows) o TeX Live (Linux/macOS).
  Paquetes: geometry, fontenc, inputenc, lmodern, microtype, xcolor, graphicx, listings
  (listingsutf8), multicol, fancyhdr, lastpage, tocloft, upquote, hyperref.
- Opcional: `pdfinfo`, `pdftotext`, `pdftoppm` (poppler) para inspeccionar los PDF.

## Uso

```
python scripts/notebook.py check [--jobs N]            # valida y compila cada snippet
python scripts/notebook.py tex                         # genera build/contents.tex y build/meta.tex
python scripts/notebook.py pdf [--theme read|print|both]   # genera los PDF (por defecto both)
```

- `check`: valida el manifiesto y los headers, y compila cada snippet con la plantilla
  (`g++ -std=gnu++17 -fsyntax-only -Wall -Wextra`). Codigos de salida: 0 ok, 1 errores de
  validacion o compilacion, 2 falta una herramienta (g++).
- `pdf`: ejecuta `tex` y luego `latexmk`. Salida: `build/testigos-de-turing-teambook.pdf`
  (lectura) y `build/testigos-de-turing-teambook-print.pdf` (impresion). Error de TeX: exit 1
  con el log relevante; sin `latexmk`: exit 2. `pdf` no corre `check`.

## Agregar un snippet

1. Crear `src/<tema>/<nombre>.cpp` con nombre en `snake_case` (`[a-z0-9_]`).
2. Escribir solo el codigo de libreria: sin `#include`, sin `using namespace std`, sin
   `main`. Usar lo que ya define `template/template.cpp` (`vi`, `pii`, `ll`, `INF`, `MOD`, ...).
3. Poner el header (ver abajo) al inicio del archivo.
4. Listarlo en `contents.txt` bajo su seccion: `src/<tema>/<nombre>.cpp | Titulo en espanol`.
5. (Opcional) Crear una demo en `tests/demo/<tema>/<nombre>.cpp`:

   ```cpp
   #include "template/template.cpp"
   #include "src/<tema>/<nombre>.cpp"
   int main() { /* ejemplo */ }
   ```
6. Correr `python scripts/notebook.py check` y luego `pdf`.

Los snippets sin entrada en `contents.txt` (y sin `Status: broken`) son un error de `check`.

## Formato del header

Comentario de bloque al inicio del snippet:

```cpp
/**
 * Uso: UnionFind UF(N); UF.unionSet(i, j); UF.isSameSet(i, j)
 * Complejidad: O(alpha(n)) amortizado
 * Fuente: (opcional)
 * Status: untested
 */
```

`Status` es obligatorio: `ok`, `untested` o `broken`. Un snippet `broken` se pone en
cuarentena: no debe aparecer en `contents.txt` (si esta listado, `check` falla) y se informa
como INFO. El contenido debe ser Latin-1 (el PDF usa `listingsutf8`); `check` informa el
codepoint ofensor.

## Gramatica de `contents.txt`

```
# comentario (linea completa) ; tambien ` #` (espacio + #) inicia un comentario en linea
[Titulo de seccion]                    # abre una seccion (nueva pagina en el PDF)
ruta/al/snippet.cpp | Titulo           # snippet: RUTA | Titulo (ruta relativa a la raiz)
!tex tex/theory/archivo.tex | Titulo   # incluye un .tex con \input
```

- La primera seccion es `[Plantilla]` con `template/template.cpp | Plantilla`.
- Rutas: `src/...*.cpp` o `template/...*.cpp` para codigo y `tex/...*.tex` para `!tex`, con caracteres `[a-z0-9_/]`; sin duplicados.
- Los errores se reportan como `contents.txt:<linea>: mensaje`.
- Los titulos pueden llevar caracteres especiales de LaTeX (`_ & % # $`); se escapan solos.
  Evitar ` #` (espacio antes de `#`) en titulos: se interpreta como comentario
  (usar `Union_Find#1`).
- Cada snippet se imprime bajo su titulo (subseccion) con la ruta en gris a la derecha; no
  hay leyendas numeradas ni lista de algoritmos (el indice de la portada cumple ese rol).

## Demos

`tests/demo/<tema>/*.cpp` son programas completos para probar a mano un snippet:

```
g++ -std=gnu++17 -I. tests/demo/dp/lcs.cpp -o lcs && ./lcs
```

## Notas

- Windows: `latexmk` (perl) requiere comillas literales en `-usepretex`; el script ya lo maneja.
- Si un `latexmk` falla y el siguiente se comporta raro, borrar `build/<tema>/`.
