#include <iostream>
/*Start with a number *n > 1*. Find the number of steps it takes
 to reach one using the following process: If *n* is even, divide it by 2. If *n* is odd, multiply it by 3 and add 1.*/
int Collatz(unsigned const int input, unsigned int step) {
    unsigned int copy = input;
    if (input < 2 && step == 0) {
        std::cout << "Please input an integer larger than 1";
        return 0;
    }
    if (input == 1) {
        return step;
    }
    if (input % 2 == 0) {
        copy = copy / 2;
        step++;
        return Collatz(copy, step);
    }
    copy = copy*3 + 1;
    step++;
    return Collatz(copy, step);
}