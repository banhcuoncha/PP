#include <iostream>
#include <functions.hpp>
int main() {
    int x;
    std::cout << "Please input a positive integer:" << std::endl;
    std::cin >> x;
    if (x <= 1) {
        std::cout << "The input number is not valid" << std::endl;
        return -1;
    } else {
        unsigned int output = Collatz(x, 0);
        std::cout << output << std::endl;
    }
    return 1;
}