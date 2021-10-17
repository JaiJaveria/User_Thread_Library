I have only implemented create, join, yield and locks.
To test it I have two codes, test.c and matmul.c.
In test locks in test.c where I have multiple threads that access the same variable. The same variable is locked, incremented and unlocked.
In matmul, I do matrix multiply on 2 matrices a and b. I have used one thread for computation of each element of resulting matrix c=a*b.