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

int num_threads = 12;
int size = 4;

TSPriorityQueue<State*, stateHash, stateEqual> open(num_threads);
std::vector<State*> path;
Lock lock;
State* start;
State* goal;


void* parallel(void* arg) {
    int thread_id = (int)(long long)(arg);
    //std::cout << thread_id << std::endl;

    std::vector<State*> tempPath;
    
    int expanded = 0;
    int notUnique = 1;

    while (1) {
        
        if (thread_id == 0 && expanded%100000 == 0) {
            for (int i = 0; i < num_threads; i++) {
                printf("%d ", open.hashSize(i));
            }
            printf("\n");
            for (int i = 0; i < num_threads; i++) {
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
                /*if (cur == NULL) {
                    std::cout << "null" << std::endl;
                } else {
                    std::cout << cur->toString() << std::endl;
                }*/
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


void runParallel() {
    //std::cout << "beginning" << std::endl;

    //int c = 0;

    pthread_t threads[num_threads];

    start = (State*)(new Board(size));
    goal = (State*)(new Board(size, 0));

    //std::cout << "here" << std::endl;

    open.push(start, NULL);

    time_t start_t = time(0);
    for (int i = 0; i < num_threads; i++) {
        //c++;
        pthread_create(&threads[i], NULL, &parallel, (void*)(long long)i);
    }

    //std::cout << c << std::endl;

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
}
