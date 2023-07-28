#include "ppm.hpp"

bool read_ppm(const std::string &filepath, std::vector<Pixel> pixels)
{
    std::ifstream f_in;
    f_in.open(filepath, std::ios::binary);
    if(!f_in)
    {
        std::cerr << "Failed to open file: " << filepath << "\n";
        return false;
    }
    std::string header;
    f_in >> header; 
    if (header != "P3") 
    {
        std::cerr << "Only P3 PPMs are supported at the moment :(\n";
        return false;
    }
    int width = 0, height = 0, max_color_val;
    f_in >> width >> height >> max_color_val;
    if (max_color_val > 255)
    {
        std::cerr << "Max color value is 255, currently showing: " << max_color_val << "\n";
        return false;
    }
    
    pixels.resize(width * height);

    std::string data;
    size_t index = 0;
    int counter = 0;
    while(f_in >> data)
    {
        if (counter % 3 == 0)
        {
            pixels[index].r = std::stoi(data);
        }
        else if (counter % 3 == 1)
        {
            pixels[index].g = std::stoi(data);
        }
        else
        {
            pixels[index].b = std::stoi(data);
            ++index;
        }
        ++counter;
    }

    return true;
}