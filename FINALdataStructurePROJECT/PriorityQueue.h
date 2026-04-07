#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

template <typename V, typename W>
class PriorityQueue {

private:
    class Node {
    public:
        V vertex;
        W dist;

        Node() : vertex(), dist() {}
        Node(V v, W d) : vertex(v), dist(d) {}
    };

    Node heap[100];      // supports up to 100 vertices
    int pos[100];
    int size;

    void swapNodes(int i, int j);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    struct Result {
        V vertex;
        W dist;
    };

    PriorityQueue(int n);
    bool empty();
    void push(V v, W d);
    void decreaseKey(V v, W newDist);
    Result extractMin();
};

/* ===== IMPLEMENTATION ===== */

template <typename V, typename W>
PriorityQueue<V, W>::PriorityQueue(int n) : size(0) {
    for (int i = 0; i < n; i++) pos[i] = -1;
}

template <typename V, typename W>
bool PriorityQueue<V, W>::empty() {
    return size == 0;
}

template <typename V, typename W>
void PriorityQueue<V, W>::swapNodes(int i, int j) {
    Node t = heap[i];
    heap[i] = heap[j];
    heap[j] = t;
    pos[heap[i].vertex] = i;
    pos[heap[j].vertex] = j;
}

template <typename V, typename W>
void PriorityQueue<V, W>::heapifyUp(int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heap[p].dist <= heap[i].dist) break;
        swapNodes(p, i);
        i = p;
    }
}

template <typename V, typename W>
void PriorityQueue<V, W>::heapifyDown(int i) {
    while (true) {
        int s = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        if (l < size && heap[l].dist < heap[s].dist) s = l;
        if (r < size && heap[r].dist < heap[s].dist) s = r;
        if (s == i) break;
        swapNodes(i, s);
        i = s;
    }
}

template <typename V, typename W>
void PriorityQueue<V, W>::push(V v, W d) {
    heap[size] = Node(v, d);
    pos[v] = size;
    heapifyUp(size++);
}

template <typename V, typename W>
void PriorityQueue<V, W>::decreaseKey(V v, W Nd) {
    int i = pos[v];
    if (i < 0) return;
    heap[i].dist = Nd;
    heapifyUp(i);
}

template <typename V, typename W>
typename PriorityQueue<V, W>::Result
PriorityQueue<V, W>::extractMin() {
    Node root = heap[0];
    heap[0] = heap[--size];
    pos[heap[0].vertex] = 0;
    heapifyDown(0);
    return { root.vertex, root.dist };
}

#endif

