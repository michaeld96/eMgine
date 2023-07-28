#ifndef MATRICES_HPP
#define MATRICES_HPP

#include "libraries.hpp"
#include <type_traits>
template <class T>
class Matrix 
{
private:
    std::vector<std::vector<T>> mat;
    void check_char()
    {
        if(std::is_same<T, char>::value) 
        {
            std::cerr << "This is not valid as character matrices cannot perform\n"
                      << "operations like matrix multiply, etc.\n";
            exit(0);
        }  
    }
public:
    // Constructor.
    Matrix(T rows, T cols) : mat(rows, std::vector<T>(cols, 0)) 
    {
        check_char();
    }
    // Fancy destructor.
    template <typename U = T,
              typename std::enable_if<!std::is_same<U, char>::value>::type* = nullptr>
    Matrix(U rows, U cols, U val) : mat(static_cast<size_t>(rows), std::vector<U>(static_cast<size_t>(cols), val)) {}
    std::vector<T>& operator[] (int index)
    {
        return mat[index];
    }
    const std::vector<T>& operator[] (int index) const
    {
        return mat[index];
    }
};

#endif // MATRICES_HPP