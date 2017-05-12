#include <iostream>
#include <vector>
#include <unordered_set>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <getopt.h>
#include <unistd.h>

#include "state.cpp"
#include "board.cpp"

State* start;
State* goal;
std::vector<State*> path;
int numThreads = 1;

#include "priorityqueue.cpp"
#include "tspriorityqueue.cpp"
#include "sequential.cpp"
#include "parallel.cpp"

int main(int argc, char *argv[]) {
    int size = 4;
    int moves = -1;
    std::string inputFile = "";
    int opt;
    while ((opt = getopt(argc, argv, "t:s:m:f:")) != -1) {
        switch (opt) {
            case 't':
                numThreads = atoi(optarg);
                break;
            case 's':
                size = atoi(optarg);
                break;
            case 'm':
                moves = atoi(optarg);
                break;
            case 'f':
                inputFile = optarg;
                break;
        }
    }

    if (inputFile.empty()) {
        if (moves >= 0) {
            start = (State*)(new Board(size, moves));
        } else {
            start = (State*)(new Board(size));
        }
    } else {
        start = (State*)(new Board(inputFile));
    }
    std::cout << "Start board:" << std::endl;
    std::cout << start->toString() << std::endl;

    goal = (State*)(new Board(size, 0));

    time_t start_t = time(0);

    if (numThreads == 1) {
        sequential();
    } else {
        parallel(numThreads);
    }

    time_t end_t = time(0);
    double time = difftime(end_t, start_t);

    std::cout << "Length of path: " << path.size()-1 << std::endl;
    std::cout << "Total time: " << time << "s" << std::endl;

    return 0;
}
