class Lock {

public: 
    pthread_mutex_t l;
    int val;

    Lock() {
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
        return (val == 1);
    }

};

TSPriorityQueue<State*, stateHash, stateEqual> open;
Lock lock;

void* parallelThread(void* arg) {
    int thread_id = (int)(long long)(arg);

    std::vector<State*> tempPath;
    
    int expanded = 0;
    int notUnique = 1;

    while (1) {
        
        if (thread_id == 0 && expanded%100000 == 0) {
            for (int i = 0; i < numThreads; i++) {
                printf("%d ", open.hashSize(i));
            }
            printf("\n");
            for (int i = 0; i < numThreads; i++) {
                printf("%d ", open.size(i));
            }
            printf("\n");
        }

        expanded++;

        State* cur = NULL;
        while (cur == NULL) {
            if (lock.test()) {
                return NULL;
            }
            cur = open.pop(thread_id);
        }

        if (*cur == *goal) {

            std::cout << "goal" << std::endl;

            if (!lock.testset()) {
                return NULL;
            } 

            while (cur != start) {
                tempPath.push_back(cur);
                cur = cur->getPrev();
            }
            tempPath.push_back(start);
            
            int pathLength = tempPath.size();
            for (int i = 0; i < pathLength; i++) {
                path.push_back(tempPath[pathLength-1-i]);
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


void parallel(int numThreads) {

    open.init(numThreads);

    pthread_t threads[numThreads];

    open.push(start, NULL);
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, &parallelThread, (void*)(long long)i);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }
}
