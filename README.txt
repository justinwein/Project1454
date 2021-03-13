The way we compiled on blue is as follows:

g++ -std=c++17 main.cpp BigUnsigned.cc BigUnsignedInABase.cc -o proj1.out

./proj1.out

main.cpp is the file that we wrote our code in. It has functions to problem 1 and problem 2.
Program gives user a choice as to which problem needs to be ran.
1 for Problem 1
2 for Problem 2
3 for Quit

We used a unlimited precision arithmetic Big Integer library from a source called Matt McCutchen. We used this code to
be able to handle larger numbers in computing the count for total number of strings of length n for Problem 1.
We had to do this because we were getting integer overflow with anything over 4 billion using unsigned ints.

The files we used from the open source are listed here: BigUnsigned.cc, BigUnsigned.h, BigUnsignedInABase.cc,
 BigUnsignedInABase.h, and NumberlikeArray.h.

Our program doesnt have any bugs as far as what the expected input and output is for the program.

For Problem 1:
Input: a value for n, 1 <= n <=300. (n needs to be between 1 and 300, including 1 and 300)
Outputs: count of the number of strings of length n

Our program creates a DFA for this problem over the alphabet {a,b,c,d} and always starts at state 0.

For Problem 2:
Input: a k value in the range of 1 to 99999.
        permitted Digits to represent the language.
Output: The smallest positive integer y > 0 that is an integer multiple of k, and has only the digits (in decimal)
 from the set S (permitted digits).
