#include "TextFile.h"

void TextFile::Trim(std::string& str) {
    const std::string delimiters = " \f\n\r\t\v";
    str.erase(str.find_last_not_of(delimiters) + 1);
    str.erase(0, str.find_first_not_of(delimiters));
}

TextFile::TextFile(const std::string& filePath) {
    std::ifstream fin(filePath);
    if (!fin.is_open()) {
        std::cerr << "Could not open file: " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(fin, line)) {
        Trim(line);
        if (!line.empty()) { // Skip blank lines
            data += line + '\n';
        }
    }
    fin.close();
}

std::string TextFile::getData() const {
    return data;
}

void TextFile::reportData() const {
    std::cout << "Data in the file:\n" << data << std::endl;
}
