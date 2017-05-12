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
