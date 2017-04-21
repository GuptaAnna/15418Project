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

	std::string toString() {
		return str;
	}

	int getHash() {
		return hashval;
	}
	
	bool operator<(State& other) {
		if (getF() < other.getF()) return true;
		if ((getF() == other.getF()) && (getG() > other.getG())) return true;
		return false;
	}

	virtual std::vector<State*> getNeighbors() = 0;

	virtual bool operator==(const State& other) = 0;

protected:
	int f;
	int g;
	int h;
	State* prev;
	bool inOpen;
	std::string str;
	int hashval;

	void initNew() {
		g = 0;
		h = computeH();
		setF();
		prev = NULL;
		inOpen = false;
		hashval = computeHash();
		str = computeStr();
	}

	void initNeighbor(State* s, int cost) {
		g = s->getG()+cost;
		h = computeH();
		setF();
		prev = s;
		inOpen = false;
		hashval = computeHash();
		str = computeStr();
	}

	void setF() {
		f = g+h;
	}

	virtual int computeH() = 0;

	virtual int computeHash() = 0;

	virtual std::string computeStr() = 0;

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

		initNew();
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
			if (numvalid == 0) { initNew(); return; }

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

		initNew();
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

private:

	// copy board and make a move
	Board(Board* b, int dx, int dy) {
		size = b->size;
		emptyRow = b->emptyRow+dx;
		emptyCol = b->emptyCol+dy;

		board = new int*[size];
		for (int i = 0; i < size; i++) {
			board[i] = new int[size];
			for (int j = 0; j < size; j++) {
				board[i][j] = b->board[i][j];
			}
		}

		board[emptyRow][emptyCol] = b->board[b->emptyRow][b->emptyCol];
		board[b->emptyRow][b->emptyCol] = b->board[emptyRow][emptyCol];

		initNeighbor((State*)b, 1);
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

	int computeHash() {
		int h = 0;
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				h = (h * 1009 + board[i][j]) % 1000003;
			}
		}
		return h;
	}

	std::string computeStr() {
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


std::vector<State*> aStar(State* start, State* goal) {
	std::vector<State*> path;
	std::vector<State*> tempPath;

	std::unordered_set<State*, stateHash, stateEqual> hash;
	
	PriorityQueue<State*> open;

	hash.insert(start);
	start->addOpen();
	open.push(start);

	int expanded = 0;
	int notUnique = 1;
	while (!open.empty()) {
		expanded++;
		State* cur = open.pop();

		int newf = cur->getF();

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
			std::unordered_set<State*>::iterator it = hash.find(neighbor);
			if (it != hash.end()) {
				neighbor = *it;
			} else {
				hash.insert(neighbor);
				neighbor->addOpen();
				open.push(neighbor);
			}

			int altG = cur->getG() + 1;
			if (neighbor->checkOpen() && altG < neighbor->getG()) {
				open.remove(neighbor);
				neighbor->setPrev(cur);
				neighbor->setG(altG);
				open.push(neighbor);
			}
		}
	}

	return path;
}



int main() {
	int size = 4;
	State* start = (State*)(new Board(size));
	State* goal = (State*)(new Board(size, 0));
	std::vector<State*> path = aStar(start, goal);
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
