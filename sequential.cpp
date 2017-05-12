std::vector<State*> sequential(State* start, State* goal) {
	std::vector<State*> path;
	std::vector<State*> tempPath;

	std::unordered_set<State*, stateHash, stateEqual> hash;
	
	PriorityQueue<State*> open;

	hash.insert(start);
	start->addOpen();
	open.push(start);

	int expanded = 0;
	int notUnique = 1;

	double duration;
	clock_t st = clock();

	while (!open.empty()) {
		if (expanded%100000 == 0) {
			duration = ( clock() - st ) / (double) CLOCKS_PER_SEC;
			std::cout << hash.size() << " " << hash.bucket_count() << " " << duration << std::endl;
			st = clock();
		}

		expanded++;
		State* cur = open.pop();

		if (*cur == *goal) {
			while (cur != start) {
				tempPath.push_back(cur);
				cur = cur->getPrev();
			}
			tempPath.push_back(start);
			
			int size = tempPath.size();
			for (int i = 0; i < size; i++) {
				path.push_back(tempPath[size-1-i]);
			}

			std::cout << "Size of open: " << open.size() << std::endl;
			std::cout << "Number of states expanded: " << expanded << std::endl;
			std::cout << "Number of states seen (unique): " << hash.size() << std::endl;
			std::cout << "Number of states seen (not unique): " << notUnique << std::endl;
			std::cout << "Length of path: " << path.size()-1 << std::endl;

			return path;
		}
		
		cur->removeOpen();
		std::vector<State*> neighbors = cur->getNeighbors();
		
		for (int i = 0; i < neighbors.size(); i++) {
			notUnique++;
			State* neighbor = neighbors[i];
			std::unordered_set<State*, stateHash, stateEqual>::iterator it = hash.find(neighbor);
			if (it != hash.end()) {
				delete neighbor;
				neighbor = *it;
			} else {
				hash.insert(neighbor);
				neighbor->addOpen();
				open.push(neighbor);
			}

			int altG = cur->getG() + 1;
			if (neighbor->checkOpen() && altG < neighbor->getG()) {
				neighbor->setPrev(cur);
				neighbor->setG(altG);
				open.update(neighbor);
			}
		}
	}

	return path;
}



void runSequential() {
	int size = 4;
	State* start = (State*)(new Board(size));
	State* goal = (State*)(new Board(size, 0));


    time_t start_t = time(0);
	std::vector<State*> path = sequential(start, goal);

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
