template <class T, class stateHash, class stateEqual>
class TSPriorityQueue {

public:

    typedef typename std::unordered_set<T, stateHash, stateEqual>::iterator iterator;

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

    int hashSize(int k) {
        pthread_mutex_lock(&locks[k]);
        int size = hashes[k].size();
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
        size_t hashval = stateHash()(element);
        //std::cout << "hashval: " << hashval << std::endl;
        return hashval % numBuckets;
    }
    
};
