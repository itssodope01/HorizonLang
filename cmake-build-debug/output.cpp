#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

int safe_divide(int numerator, int denominator) {
    if (denominator == 0) {
        throw std::runtime_error("Division by zero");
    }
    return numerator / denominator;
}

    int result = power(4, 2);
    int resultb = power(2, 3);

int power(int a, int b) {
        int result = 1;
        {
            int __start = 0;
            int __end = b;
            int __step = (__start < __end ? 1 : -1);
            for (int i = __start; (__step > 0 ? i < __end : i > __end); i += __step) {
                result = (result * a);
            }
        }
        return result;
}

int main() {
        std::cout << result << std::endl;
        std::cout << resultb << std::endl;
    return 0;
}
