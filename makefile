all:
	c++ -std=c++11 -pthread -o run main.cpp

clean:
	$(RM) run