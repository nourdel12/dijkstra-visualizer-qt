#include"PriorityQueue.h"
#include <iostream>
using namespace std;

const int MAX = 100;
const int INF = 1000000000;


// ------------------- Dijkstra Algorithm -------------------
void dijkstra(int graph[MAX][MAX], int n, int src)
{
    double dist[MAX];         // double eliminates conversion warnings
    bool visited[MAX] = {};   // initialize all elements to false

    // Initialize distances
    for (int i = 0; i < n; i++) {
        dist[i] = INF;        // INF will be promoted safely to double
    }

    dist[src] = 0;

    // PriorityQueue< VertexType=int , DistanceType=double 
    PriorityQueue<int, double> pq(n);

    // Insert all vertices into PQ
    for (int i = 0; i < n; i++)
        pq.push(i, dist[i]);

    // Dijkstra loop
    while (!pq.empty())
    {
        auto r = pq.extractMin();
        int u = r.vertex;

        if (visited[u])
            continue;

        visited[u] = true;

        // Relax all outgoing edges
        for (int v = 0; v < n; v++)
        {
            if (graph[u][v] != INF && !visited[v])
            {
                double newDist = dist[u] + (double)graph[u][v];

                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    pq.decreaseKey(v, newDist);
                }
            }
        }
    }

    // Output final distances
    cout << "\nShortest distances from vertex " << src << ":\n";
    for (int i = 0; i < n; i++) {
        cout << src << " -> " << i << " = " << dist[i] << '\n';
    }
}
