#ifndef PPM_HPP
#define PPM_HPP

#include "libraries.hpp"
struct Pixel
{
    int r;
    int g;
    int b;
};

bool read_ppm(const std::string &filepath, std::vector<Pixel> pixels);

#endif // M_HPP