void sequential() {
	std::vector<State*> tempPath;

	std::unordered_set<State*, stateHash, stateEqual> hash;

	PriorityQueue<State*> open;

	hash.insert(start);
	start->addOpen();
	open.push(start);

	int expanded = 0;

	while (!open.empty()) {
		if (expanded%100000 == 0) {
			printf("...\n");
		}

		expanded++;
		State* cur = open.pop();

		if (*cur == *goal) {
			while (cur != start) {
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
		
		cur->removeOpen();
		std::vector<State*> neighbors = cur->getNeighbors();
		
		for (int i = 0; i < neighbors.size(); i++) {
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

	return;
}