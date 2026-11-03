/**
 * Uso: vvi cap = minimaxAllPairs(C, graph); cap[s][t] = menor valor posible del peso maximo en un camino s -> t (graph: lista de adyacencia de (nodo, peso), 0-indexada)
 * Complejidad: O(C^2 + L) segun el autor original (en realidad C Dijkstra: O(C * L log C))
 * Status: untested
 */
typedef vector<vpii> vvpii;

vvi minimaxAllPairs(int C, const vvpii &graph){
    // Para cada fuente i hacemos un "minimax-Dijkstra"
    // y llenamos cap[i][*]
    vvi cap(C, vi(C));
    for (int src = 0; src < C; src++){
      // vis[v]=si ya fijamos cap[src][v]
      vector<char> vis(C, 0);
      // pq minimo por peso máximo en el camino
      priority_queue<pii, vpii, greater<pii>> pq;

      // (peso_actual, nodo)
      pq.emplace(0, src);
      while (!pq.empty()){
        auto [bestW, u] = pq.top();
        pq.pop();
        if (vis[u])
          continue;
        vis[u] = 1;
        cap[src][u] = bestW;
        for (auto &e : graph[u])
        {
          int v = e.first, w = e.second;
          if (!vis[v])
          {
            // el peso del camino a v es el máximo
            // entre el acumulado y la arista (u,v)
            int newW = max(bestW, w);
            pq.emplace(newW, v);
          }
        }
      }
    }
    return cap;
}
