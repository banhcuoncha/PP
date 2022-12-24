#include <iostream>
/*Start with a number *n > 1*. Find the number of steps it takes
 to reach one using the following process: If *n* is even, divide it by 2. If *n* is odd, multiply it by 3 and add 1.*/
int Collatz(const int input) {
    int copy = input;
    int steps = 0;
    if (input < 2) {
        std::cout << "Please input an integer larger than 1";
        return;
    }
    if (input % 2 == 0) {
        while (copy != 1) {
            copy = copy / 2;
            steps++;
        }
        return steps;
    }

}