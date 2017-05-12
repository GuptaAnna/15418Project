## How to Run Our Program

### Running a File

In order to run the algorithm on a specific board input, we can encode the board input in a file. 

The layout of the file must be like: <br />
```
3 
1 3 4 
2 5 6
8 7 0
```

The first line is the width of the board. The following files are the board input, with the 0 encoding the empty space. 

Then, you can type the following into the command line:
```
    make
    ./run -t numThreads -f filename
```

_numThreads_ should be the number of threads working on the algorithm. _filename_ is the full name of your file. We have sample files you can use in the _tests_ folder. 

An example: 
```
    make
    ./run -t 12 -f tests/med1.txt
```

### Randomized Input

We can also run the algorithm on a randomized input, generated by the program. You have 3 flags available: 

- t: The number of threads working on the algorithm. The default value is 1. 
- m: The system will begin with the goal board and perform _m_ randomly generated moves. A greater _m_ signifies a more randomized board. 
- s: The width of the board. The default value is 4. 

An example: 
```
    make
    ./run -t 12 -m 200 -s 3 
    ```
