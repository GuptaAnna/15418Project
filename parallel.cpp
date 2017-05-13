TSPriorityQueue<State*, stateHash, stateEqual> open;
int lock = 0;
int numBuckets;

bool handle_tick() {
    State* element;
    if ((element = open.find(goal)) == NULL) {
        return false;
    }

    int fval = element->getF();

    for (int i = 0; i < numBuckets; i++) {
        if (open.getMinKey(i) < fval) {
            return false;
        }
    }
    return true;
}

void* parallelThread(void* arg) {
    int thread_id = (int)(long long)(arg);
    int expanded = 0;

    while (1) {
        
        if (thread_id == 0 && expanded%10000 == 0) {
            if (expanded % 100000 == 0) {
                printf("Finding optimal solution...\n");
            }
            if (handle_tick()) {
                lock = 1;
                return NULL;
            }
        }

        expanded++;

        State* cur = NULL;
        while (cur == NULL) {
            if (lock == 1) {
                return NULL;
            }
            if (bucketMultiplier == -1) {
                cur = open.pop(thread_id);
            } else {
                cur = open.pop(rand()%numBuckets);
            }
        }
        
        cur->removeOpen();
        std::vector<State*> neighbors = cur->getNeighbors();
        
        for (int i = 0; i < neighbors.size(); i++) {
            State* neighbor = neighbors[i];
            open.push(neighbor, cur);
        }
    }

    return NULL;
}

void createPath() {
    std::vector<State*> tempPath;

    State* cur = open.find(goal);

    while (cur != NULL && cur != start) {
        tempPath.push_back(cur);
        cur = cur->getPrev();
    }
            
    tempPath.push_back(start);
            
    int pathLength = tempPath.size();
    for (int i = 0; i < pathLength; i++) {
        path.push_back(tempPath[pathLength-1-i]);
    }

    return;
}


void parallel(int numThreads) {

    if (bucketMultiplier == -1) {
        numBuckets = numThreads;
    } else {
        numBuckets = bucketMultiplier*numThreads;
    }

    srand(time(NULL));

    open.init(numBuckets);

    pthread_t threads[numThreads];

    open.push(start, NULL);
    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, &parallelThread, (void*)(long long)i);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    createPath();
}
