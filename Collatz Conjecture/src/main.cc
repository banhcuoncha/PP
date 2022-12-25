#include <iostream>
#include <functions.hpp>
int main() {
    unsigned int x;
    std::cout << "Please input a positive integer:" << std::endl;
    std::cin >> x;
    if (x <= 1) {
        std::cout << "The input number is not valid" << std::endl;
    } else {
        unsigned int output = Collatz(x, 0);
        std::cout << output << std::endl;
    }
}