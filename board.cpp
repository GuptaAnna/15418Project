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

    // make moves from goal board
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

    // initialize board based on file
    Board(std::string s) {
        FILE *fp = fopen(s.c_str(), "r");
        fscanf(fp, "%d", &size);

        board = new int*[size];
        for (int i = 0; i < size; i++) {
            board[i] = new int[size];
            int row = i*size;
            for (int j = 0; j < size; j++) {
                fscanf(fp, "%d", &board[i][j]);
                if (board[i][j] == 0) {
                    emptyRow = i;
                    emptyCol = j;
                }
            }
        }
        fclose(fp);

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
        /*
        s += "Board of size " + std::to_string(size) + ":\n";
        s += "f=" + std::to_string(f) + ", ";
        s += "g=" + std::to_string(g) + ", ";
        s += "h=" + std::to_string(h) + "\n";
        s += "inOpen=" + std::to_string(inOpen) + "\n";
        s += "hashval=" + std::to_string(hashval) + "\n";
        */
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

        size_t v = h;
        v = v ^ (v >> 21);
        v = v ^ (v << 37);
        v = v ^ (v >> 4);
        v = v * 4768777513237032717;
        v = v ^ (v << 20);
        v = v ^ (v >> 41);
        v = v ^ (v <<  5);
        return v;
    }

};
