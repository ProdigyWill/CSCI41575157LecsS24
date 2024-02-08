#ifndef TEXTFILE_H
#define TEXTFILE_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class TextFile {
private:
    std::string data;

    void Trim(std::string& str);

public:
    TextFile(const std::string& filePath);

    std::string getData() const;

    void reportData() const;
};
#endif
