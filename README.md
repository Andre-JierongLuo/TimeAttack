The goal is to study a simple timing attack.

time program performs a timing attack against the mynuke program and print the correct launch code.

gcc -std=gnu99 time.c -lrt -o time

mem_eq function compares the first n bytes of memory at m1 to the first n bytes at m2. And its implementation would not be vulnerable to a timing attack.
