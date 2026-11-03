#include "template/template.cpp"
#include "src/graphs/trees/minimax_all_pairs.cpp"

int main(){

  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  while (true){
    int C, L, H;
    cin >> C >> L >> H;
    if (!cin || (C == -1 && L == -1 && H == -1)) break;

    // 1) Leer grafo
    vvpii graph(C);
    for (int i = 0; i < L; i++){
      int a, b, w; cin >> a >> b >> w;
      --a,--b;
      graph[a].emplace_back(b, w);
      graph[b].emplace_back(a, w);
    }

    // 2) Para cada fuente i hacemos un "minimax-Dijkstra"
    vvi cap = minimaxAllPairs(C, graph);

    // 3) Responder consultas
    int s, t; cin >> s >> t;
    
    cout << cap[s - 1][t - 1];
    for (int i = 1; i < H; i++){
      cin >> s >> t;
      cout << ' ' << cap[s - 1][t - 1];
    }
    cout << "\n";
  }

  return 0;
}
