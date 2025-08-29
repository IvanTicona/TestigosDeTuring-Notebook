---
title: Min Max Tree Query O(1)
category: trees
author: Testigos de Turing
description: Complejidad C^2 + L, donde C son los nodos y L aristas
---
/*
Min Max Tree Query - Versión optimizada O(1)
*/

#include <bits/stdc++.h>

using namespace std;

typedef vector<int> vi;
typedef pair<int, int> ii;
typedef vector<ii> vii;
typedef vector<vi> vvi;
typedef vector<vii> vvii;

int main(){

  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  while (true){
    int C, L, H;
    cin >> C >> L >> H;
    if (!cin || (C == -1 && L == -1 && H == -1)) break;

    // 1) Leer grafo
    vvii graph(C);
    for (int i = 0; i < L; i++){
      int a, b, w; cin >> a >> b >> w;
      --a,--b;
      graph[a].emplace_back(b, w);
      graph[b].emplace_back(a, w);
    }

    // 2) Para cada fuente i hacemos un "minimax‐Dijkstra"
    //    y llenamos cap[i][*]
    vvi cap(C, vi(C));
    for (int src = 0; src < C; src++){
      // vis[v]=si ya fijamos cap[src][v]
      vector<char> vis(C, 0);
      // pq minimo por peso máximo en el camino
      priority_queue<ii, vii, greater<ii>> pq;

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
