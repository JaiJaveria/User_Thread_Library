# User Level Thread Library
- In this repository I have made an user level thread library and have implemented create, join, yield for threads as well as locks.
- To test it I have two codes, test.c and matmul.c.
- To test locks, I have test.c where I have multiple threads that access the same variable. The same variable is locked, incremented and unlocked.
- In matmul, I do matrix multiply on 2 matrices a and b. I have used one thread for computation of each element of resulting matrix c=a*b.

## Usage Instructions
- Clone the repository on your local computer.
- To compile the files, do ```make tests```
- Run the executable in the bin/ folder
```
bin/matmul
bin/locks <number of threads>
```
