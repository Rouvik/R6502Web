#include <iostream>
#include <string>
#include <fstream>

int hexToBin(std::string path, std::string outputPath)
{
    std::ifstream in(path);
    if (!in)
    {
        std::cerr << "[Error] Failed to open file for input\n";
        return 1;
    }
    
    std::ofstream out(outputPath);
    if (!out)
    {
        std::cerr << "[Error] Failed to open file for output\n";
        return 1;
    }
    
    while (!in.eof())
    {
        std::string hexToken;
        in >> hexToken;

        char byte = std::stoi(hexToken, 0, 16);
        out.write(&byte, 1);
    }
    
    in.close();
    out.close();

    std::cout << "Converted successfully\n";

    return 0;
}

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Error missing parameters\nUsage: " << argv[0] << " <input hex file> <output bin file>\n";
        return 1;
    }
    
    return hexToBin(argv[1], argv[2]);
}