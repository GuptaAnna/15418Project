#include <iostream>
#include <vector>
#include <unordered_set>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>


class State {

public:
    int heapIdx;
    
    int getF() {
        return f;
    }

    void setG(int gValue) {
        g = gValue;
        setF();
    }

    int getG() {
        return g;
    }

    int getH() {
        return h;
    }

    void setPrev(State* s) {
        prev = s;
    }

    State* getPrev() {
        return prev;
    }

    void addOpen() {
        inOpen = true;
    }

    void removeOpen() {
        inOpen = false;
    }

    bool checkOpen() {
        return inOpen;
    }

    size_t getHash() {
        return hashval;
    }
    
    bool operator<(State& other) {
        if (getF() < other.getF()) return true;
        if ((getF() == other.getF()) && (getG() > other.getG())) return true;
        return false;
    }

    virtual std::vector<State*> getNeighbors() = 0;

    virtual bool operator==(const State& other) = 0;

    virtual std::string toString() = 0;

    virtual ~State() {}

protected:
    int f;
    int g;
    int h;
    State* prev;
    bool inOpen;
    size_t hashval;

    void initNew(int gval) {
        g = gval;
        h = computeH();
        setF();
        prev = NULL;
        inOpen = false;
        hashval = computeHash();
    }

    void setF() {
        f = g+h;
    }

    virtual int computeH() = 0;

    virtual size_t computeHash() = 0;

};



class Board : public State {

public:
    int** board;
    int size;
    int emptyRow;
    int emptyCol;

    // random board
    Board(int n) {
        size = n;
        int size2 = size * size;

        // creates random permutation
        int* permutation = new int[size2];
        for (int i = 0; i < size2; i++) {
            permutation[i] = i;
        }
        srand(time(NULL));
        for (int i = size2; i > 0; i--) {
            int swapidx = rand() % i;
            int tmp = permutation[swapidx];
            permutation[swapidx] = permutation[i-1];
            permutation[i-1] = tmp;
        }

        // compute sign of permutation
        int sign = 0;
        for (int i = 0; i < size2; i++) {
            int ival = permutation[i] == 0 ? size2 : permutation[i];
            for (int j = i+1; j < size2; j++) {
                int jval = permutation[j] == 0 ? size2 : permutation[j];
                if (ival > jval) sign++;
            }
            if (ival == size2) {
                sign += size-1 - i/size;
                sign += size-1 - i%size;
            }
        }
        // if sign of permutation is odd, make the permutation valid
        if (sign%2 == 1) {
            int idx0 = 0, idx1 = 1;
            if (permutation[0] == 0) { idx0 = 1; idx1 = 2; }
            else if (permutation[1] == 0) { idx1 = 2; }
            int tmp = permutation[idx0];
            permutation[idx0] = permutation[idx1];
            permutation[idx1] = tmp;
        }

        // insert permutation into board
        board = new int*[size];
        for (int i = 0; i < size; i++) {
            board[i] = new int[size];
            int row = i*size;
            for (int j = 0; j < size; j++) {
                board[i][j] = permutation[row+j];
                if (board[i][j] == 0) {
                    emptyRow = i;
                    emptyCol = j;
                }
            }
        }
        delete[] permutation;

        initNew(0);
    }

    // goal board
    Board(int n, int moves) {
        size = n;

        board = new int*[size];
        for (int i = 0; i < size; i++) {
            board[i] = new int[size];
            int row = 1+i*size;
            for (int j = 0; j < size; j++) {
                board[i][j] = row+j;
            }
        }
        board[size-1][size-1] = 0;
        emptyRow = size-1;
        emptyCol = size-1;

        //make random moves
        srand(time(NULL));
        while(moves--) {
            int dx[4] = {-1, 1, 0, 0};
            int dy[4] = {0, 0, -1, 1};
            int valid[4] = {0, 0, 0, 0};

            int numvalid = 0;
            if (emptyRow > 0) { valid[0] = 1; numvalid++;}
            if (emptyRow < size-1) { valid[1] = 1; numvalid++;}
            if (emptyCol > 0) { valid[2] = 1; numvalid++;}
            if (emptyCol < size-1) { valid[3] = 1; numvalid++;}
            if (numvalid == 0) { initNew(0); return; }

            int idx = 0;
            int swapidx = rand() % numvalid;
            for (int i = 0; i <= swapidx; i++) {
                if (i > 0) idx++;
                while (valid[idx] == 0) idx++;
            }

            int newEmptyRow = emptyRow + dx[idx];
            int newEmptyCol = emptyCol + dy[idx];
            board[emptyRow][emptyCol] = board[newEmptyRow][newEmptyCol];
            board[newEmptyRow][newEmptyCol] = 0;
            emptyRow = newEmptyRow;
            emptyCol = newEmptyCol;
        }

        initNew(0);
    }

    std::vector<State*> getNeighbors() {
        std::vector<State*> neighbors;
        if (emptyRow > 0) {
            Board* nbr = new Board(this, -1, 0);
            neighbors.push_back((State*)nbr);
        }
        if (emptyRow < size-1) {
            Board* nbr = new Board(this, 1, 0);
            neighbors.push_back((State*)nbr);
        }
        if (emptyCol > 0) {
            Board* nbr = new Board(this, 0, -1);
            neighbors.push_back((State*)nbr);
        }
        if (emptyCol < size-1) {
            Board* nbr = new Board(this, 0, 1);
            neighbors.push_back((State*)nbr);
        }
        return neighbors;
    }

    bool operator==(const State& other) {
        Board* b = (Board*)(&other);
        if (size != b->size) return false;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (board[i][j] != b->board[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    std::string toString() {
        std::string s;
        s += "Board of size " + std::to_string(size) + ":\n";
        s += "f=" + std::to_string(f) + ", ";
        s += "g=" + std::to_string(g) + ", ";
        s += "h=" + std::to_string(h) + "\n";
        s += "inOpen=" + std::to_string(inOpen) + "\n";
        s += "hashval=" + std::to_string(hashval) + "\n";
        s += "emptyRow=" + std::to_string(emptyRow) + " ";
        s += "emptyCol=" + std::to_string(emptyCol) + "\n";
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                s += std::to_string(board[i][j]);
                s += " ";
            }
            s += "\n";
        }
        return s;
    }

private:

    // copy board and make a move
    Board(Board* b, int dx, int dy) {
        size = b->size;
        emptyRow = b->emptyRow+dx;
        emptyCol = b->emptyCol+dy;

        board = new int*[size];
        for (int i = 0; i < size; i++) {
            if (i == emptyRow || i == b->emptyRow) {
                board[i] = new int[size];
                for (int j = 0; j < size; j++) {
                    board[i][j] = b->board[i][j];
                }
            } else {
                board[i] = b->board[i];
            }
        }

        board[emptyRow][emptyCol] = b->board[b->emptyRow][b->emptyCol];
        board[b->emptyRow][b->emptyCol] = b->board[emptyRow][emptyCol];

        initNew(INT_MAX);
    }

    int computeH() {
        int total = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                int element = board[i][j];
                if (i == emptyRow && j == emptyCol) continue;
                int row = (element-1)/size;
                int col = (element-1)%size;
                int distance = abs(row-i) + abs(col-j);
                total += distance;
            }
        }
        return total;
    }

    size_t computeHash() {
        size_t h = 0;
        int size2 = size * size;
        size_t mod = (1LL<<56)-5;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                h = (h * size + board[i][j]) % mod;
            }
        }
        return h;
    }

};

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
        T element = heap[1];
        swap(1, heap.size()-1);
        heap.pop_back();
        if (1 < heap.size()) {
            siftDown(1);
        }
        return element;
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


template <class T, class stateHash, class stateEqual>
class TSPriorityQueue {

public:

    typedef typename std::unordered_set<T>::iterator iterator;

    TSPriorityQueue(int n) {
        numBuckets = n;
        locks = (pthread_mutex_t*)malloc(n * sizeof(pthread_mutex_t));
        for (int k = 0; k < n; k++) {
            pthread_mutex_init(&locks[k], NULL);
            pqs.push_back(PriorityQueue<T>());
            hashes.push_back(std::unordered_set<T, stateHash, stateEqual>());
        }
    }

    bool empty(int k) {
        pthread_mutex_lock(&locks[k]);
        bool b = pqs[k].empty();
        pthread_mutex_unlock(&locks[k]);
        return b;
    }
    
    int size(int k) {
        pthread_mutex_lock(&locks[k]);
        int size = pqs[k].size();
        pthread_mutex_unlock(&locks[k]);
        return size;
    }

    void push(T element, State* cur) {
        int k = bucketNum(element);
        pthread_mutex_lock(&locks[k]);
        int altG;
        if (cur == NULL) {
            altG = INT_MAX;
        } else { 
            altG = cur->getG() + 1;
        } 

        iterator it = hashes[k].find(element);
        if (it != hashes[k].end()) {
            // found existing state
            delete element;
            element = *it;

            if (altG < element->getG()) {
                element->setPrev(cur);
                element->setG(altG);
                if (element->checkOpen()) {
                    pqs[k].update(element);            
                } else {
                    pqs[k].push(element);
                }
            }
        } else {
            // new state
            hashes[k].insert(element);
            element->addOpen();
            
            element->setPrev(cur);
            element->setG(altG);
            pqs[k].push(element);
        }
        pthread_mutex_unlock(&locks[k]);
    }
    
    T pop(int k) {
        pthread_mutex_lock(&locks[k]);
        T element = pqs[k].pop();
        pthread_mutex_unlock(&locks[k]);
        return element;
    }

    void remove(T element) {
        int k = bucketNum(element);
        pthread_mutex_lock(&locks[k]);
        pqs[k].remove(element);
        pthread_mutex_unlock(&locks[k]);
    }

    void update(T element) {
        int k = bucketNum(element);
        pthread_mutex_lock(&locks[k]);
        pqs[k].update(element);
        pthread_mutex_unlock(&locks[k]);
    }

private:

    pthread_mutex_t* locks;
    std::vector< PriorityQueue<T> > pqs;
    std::vector< std::unordered_set<T, stateHash, stateEqual> > hashes;
    int numBuckets;

    int bucketNum(T element) {
        int hashval = stateHash()(element);
        return hashval % numBuckets;
    }

};


struct stateHash {
public:
    size_t operator()(State* s) const {
        return s->getHash();
    }
};

struct stateEqual {
public:
    bool operator()(State* s1, State* s2) const {
        return *s1 == *s2;
    }
};

class Lock {

public: 
    pthread_mutex_t l;
    int val;

    Lock() {
        // unlocked
        val = 0;
        pthread_mutex_init(&l, NULL);
    }

    bool testset() {
        bool result = false;
        pthread_mutex_lock(&l);
        if (val == 0) {
            val = 1;
            result = true;
        } 
        pthread_mutex_unlock(&l);
        return result;
    }

    bool test() {
        bool result = false;
        pthread_mutex_lock(&l);
        if (val == 1) {
            result = true;
        } 
        pthread_mutex_unlock(&l);
        return result;
    }

};

TSPriorityQueue<State*, stateHash, stateEqual> open(1);
std::vector<State*> path;
Lock lock;
State* start;
State* goal;

void* aStar(void* arg) {

    

    //open.push(start);


    //std::vector<State*> path;
    std::vector<State*> tempPath;
    
    int expanded = 0;
    int notUnique = 1;

    double duration;
    std::clock_t st = std::clock();

    while (!open.empty(0)) {
        if (lock.test()) {
            return NULL;
        }

        if (expanded%100000 == 0) {
            duration = ( std::clock() - st ) / (double) CLOCKS_PER_SEC;
            std::cout << duration << std::endl;
            st = std::clock();
        }

        expanded++;
        State* cur = open.pop(0);

        if (*cur == *goal) {

            std::cout << "goal" << std::endl;

            if (!lock.testset()) {
                return NULL;
            } 

            while (cur != start) {
                tempPath.push_back(cur);
                if (cur == NULL) {
                    std::cout << "null" << std::endl;
                }
                else {
                    std::cout << cur->toString() << std::endl;
                }
                cur = cur->getPrev();
            }
            tempPath.push_back(start);
            
            int size = tempPath.size();
            for (int i = 0; i < size; i++) {
                path.push_back(tempPath[size-1-i]);
            }

            std::cout << "Size of open: " << open.size(0) << std::endl;
            std::cout << "Number of states expanded: " << expanded << std::endl;
            std::cout << "Number of states seen (not unique): " << notUnique << std::endl;
            std::cout << "Length of path: " << path.size()-1 << std::endl;

            return NULL;
        }
        
        cur->removeOpen();
        std::vector<State*> neighbors = cur->getNeighbors();
        
        for (int i = 0; i < neighbors.size(); i++) {
            notUnique++;
            State* neighbor = neighbors[i];
            open.push(neighbor, cur);
        }
    }

    return NULL;
}



int main() {
    std::cout << "beginning" << std::endl;

    int c = 0;
    int size = 4;
    int num_threads = 10;
    pthread_t threads[num_threads];

    start = (State*)(new Board(size));
    goal = (State*)(new Board(size, 0));

    std::cout << "here" << std::endl;

    open.push(start, NULL);

    time_t start_t = time(0);
    for (int i = 0; i < num_threads; i++) {
        c++;
        pthread_create(&threads[i], NULL, aStar, (void*)(long long)i);
    }

    std::cout << c << std::endl;

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    time_t end_t = time(0);
    double time = difftime(end_t, start_t);

    std::cout << "Time: " << time << std::endl;

    /*
    for (int i = 0; i < path.size(); i++) {
        std::cout << path[i]->toString() << std::endl;
    }
    std::cout << "Length of path: " << path.size()-1 << std::endl;
    */

    /*
    std::cout << start->toString() << std::endl;
    std::cout << goal->toString() << std::endl;
    std::vector<State*> neighbors = start->getNeighbors();
    for (int i = 0; i < neighbors.size(); i++) {
        std::cout << neighbors[i]->toString() << std::endl;
    }
    */
    return 0;
}
