#pragma once
#include "anthraxAI/vkdefines.h"
#include <fstream>
#include <typeinfo>

enum Elements {
    PARSE_ID = 0,
    PARSE_PATH = 1,
    PARSE_FOLLOW = 1,
    PARSE_COLLISION,
    PARSE_X,
    PARSE_Y,
    PARSE_FPS,
    PARSE_TYPE,
    PARSE_ANIMATION,
    PARSE_TEXT,
    PARSE_POSITION,
};

class Parser {
    public:
        bool Load(const std::string& filen);
        void Clear() { file.clear(); }

        template <typename T>
        T GetElement(Elements element);
        bool GetElement(std::string element);
    private:
        std::vector<std::string>::iterator currentelement;
        std::string Filename;
        std::vector<std::string> file;
};

template <typename T>
T Parser::GetElement(Elements element) {
    std::vector<std::string>::iterator data = currentelement + element;
    std::string modify = *data;
    std::size_t found = modify.find_first_of(":") + 2;
    modify.erase(0, found);

    std::stringstream ss{modify};
    T type;
    ss >> type;
    return type;
}