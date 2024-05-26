#include "anthraxAI/parser.h"

bool Parser::Load(const std::string& filen)
{
    std::ifstream read;
    Filename = filen;
    read.open(Filename);
    std::string modify;
    file.reserve(128);//randmly
    while (std::getline(read, modify)) {
        file.push_back(modify);
    }
    read.close();
    currentelement = file.begin();
    return true;
}

bool Parser::GetElement(std::string element) {
    std::vector<std::string>::iterator it = std::find(currentelement, file.end(), element);
    bool found = it != file.end();
    if (found) {
        currentelement = ++it;
    }
    return found;
}