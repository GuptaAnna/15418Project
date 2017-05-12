template <class T>
class PriorityQueue {

public:
    
    PriorityQueue() {
        heap.push_back(NULL);
    }
    
    bool empty() {
        return heap.size() == 1;
    }
    
    int size() {
        return heap.size()-1;
    }
    
    void push(T element) {
        heap.push_back(element);
        element->heapIdx = heap.size()-1;
        siftUp(heap.size()-1);
    }
    
    T pop() {
        if (!empty()) {
            T element = heap[1];
            swap(1, heap.size()-1);
            heap.pop_back();
            if (1 < heap.size()) {
                siftDown(1);
            }
            return element;
        }
        return NULL;
    }
    
    void remove(T element) {
        int idx = element->heapIdx;
        swap(idx, heap.size()-1);
        heap.pop_back();
        if (idx < heap.size()) {
            siftUp(idx);
            siftDown(idx);
        }
    }

    void update(T element) {
        int idx = element->heapIdx;
        siftUp(idx);
        siftDown(idx);
    }

private:

    std::vector<T> heap;
    
    void swap(int idx1, int idx2) {
        T tmp = heap[idx1];
        heap[idx1] = heap[idx2];
        heap[idx2] = tmp;
        heap[idx1]->heapIdx = idx1;
        heap[idx2]->heapIdx = idx2;
    }
    
    void siftUp(int idx) {
        int parentIdx = idx/2;
        if ((parentIdx >= 1) && (*(heap[idx]) < *(heap[parentIdx]))) {
            swap(idx, parentIdx);
            siftUp(parentIdx);
        }
    }
    
    void siftDown(int idx) {
        int childIdx = idx;
        if ((2*idx < heap.size()) && (*(heap[2*idx]) < *(heap[childIdx]))) {
            childIdx = 2*idx;
        }
        if ((2*idx+1 < heap.size()) && (*(heap[2*idx+1]) < *(heap[childIdx]))) {
            childIdx = 2*idx+1;
        }
        if (idx != childIdx) {
            swap(idx, childIdx);
            siftDown(childIdx);
        }
    }
 
};
