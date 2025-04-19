#include "anthraxAI/utils/parser.h"
#include "anthraxAI/utils/defines.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <string>

std::string Utils::Parser::ConsrtuctElementName(const LevelElements& element) const
{
    std::string name = Utils::GetValue(element) + ":";
    return name;
}

void Utils::Parser::PrintTokenized()
{
 int i = 0;
    printf("-----------------TOKENIZED--------------------------\n");
    for (auto& it : Tokens) {
        printf("[%d][%s][%s]\n", i, it.first.c_str(), it.second.c_str());
        i++;
    }
    printf("----------------------------------------------------\n");

}

void Utils::Parser::Write(const std::string& filename)
{
    std::ofstream write;
    write.open(LEVEL_PATH + filename);

    ASSERT(!write.is_open(), ("Failed to open a file " + filename).c_str());

    for (auto& it : Tokens) {
        if (it.second.empty()) {
            write << it.first + ":" << std::endl;
        }
        else {
            write << it.first + ": " + it.second << std::endl;
        }
    }
    write << std::endl;
    write.close();
    printf("File [%s] overwritten\n", filename.c_str() );
}

void Utils::Parser::Load(const std::string& filename)
{
    std::ifstream read;
    read.open(LEVEL_PATH + filename);
    ASSERT(!read.is_open(), ("Failed to open a file " + filename).c_str());

    File.reserve(256);
    std::string str, strsub;
    std::size_t foundlast = 0;

    while (std::getline(read, str)) {
        File.emplace_back(str);
    }

    read.close();

    Tokenize(File.begin());
    RootNode = Tokens.begin();
    ChildRange = Tokens.end();

    PrintTokenized();
}

void Utils::Parser::Tokenize(std::vector<std::string>::const_iterator it)
{
    Tokens.reserve(File.size());

    char delimeter = ':';
    std::string key;
    std::string value;
    bool delimfound = false;

    ASSERT(it == File.end(), "Utils::Parser::Tokenize() file is empty!");

    for (; it != File.end(); ++it) {
        if (*it == "\n" || (*it).empty()) continue;
        for (std::string::const_iterator ch = it->begin(); ch != it->end(); ++ch) {
            while (*ch == '\0' || *ch == ' ' || *ch == '\t' || *ch == '\n') {
                ++ch;
            }
            if (*ch == ':') {
                delimfound = true;
            }
            if (!delimfound) {
                key += *ch;
            }
            else if (*ch != ':') {
                value += *ch;
            }
        }
        ASSERT(!delimfound, "Utils::Parser::Tokenize() missing ':'");
        Tokens.emplace_back(std::make_pair(key, value));
        key.clear();
        value.clear();
        delimfound = false;
    }
    ASSERT(Tokens.empty(), "Utils::Parser::Tokenize() file was empty");
}

Utils::NodeIt Utils::Parser::GetChildByID(const NodeIt& node, const std::string& id)  const
{
    std::string key =  Utils::GetValue(Utils::LEVEL_ELEMENT_ID);

    NodeIt it = std::find_if(node, Tokens.end(), [key, id](const auto& iter) { return iter.first == key && iter.second == id; });
    if (it == Tokens.end()) {

        std::cout << ("Parser::GetChildByID(): Child not found |" + key + "| id:" + id) << std::endl;
    }
    return it;
}

Utils::NodeIt Utils::Parser::GetChild(const NodeIt& node, const LevelElements& elem) const
{
    std::string key =  Utils::GetValue(elem);
    NodeIt obj_it = Tokens.end();

    if (elem == Utils::LEVEL_ELEMENT_ANIMATION) {
        std::string obj_key = Utils::GetValue(Utils::LEVEL_ELEMENT_OBJECT);
        obj_it = std::find_if(node, Tokens.end(), [obj_key](const auto& n) { return n.first == obj_key; } );
        const_cast<NodeIt&>(ChildRange) = obj_it;
    }

    NodeIt it = std::find_if(node, obj_it, [key](const auto& n) { return n.first == key; } );

    if (it == Tokens.end()) {
        std::cout << ("Parser::GetElement(): Child not found |" + key + "|") << std::endl;
    }
    return it;
}

std::string Utils::Parser::GetRootElement() const
{
    ASSERT((RootNode->first) != Utils::GetValue(Utils::LEVEL_ELEMENT_SCENE), "Parser::GetRootElement(): Root Node != 'Scene'");
    std::string root = RootNode->second;
    return root;
}
