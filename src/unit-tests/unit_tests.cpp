#include "unit_tests.hpp"
// // #include <iostream>
void test_matrix_construction()
{
    std::cout << "Testing Matrices\n";
    Matrix<int> M(4,4);
    assert(M[0][0] == 0);
    Matrix<double> M1(4.0,4.0, 2.0);

    std::cout << "All tests passed! :)\n";
}