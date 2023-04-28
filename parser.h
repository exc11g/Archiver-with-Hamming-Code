#include <vector>
#include <string>
#include <iostream>

#ifndef LABWORK_4_EXC11G_PARSER_H

#define LABWORK_4_EXC11G_PARSER_H

struct Parser {
    std::vector<std::string> fileNames;
    std::vector<std::string> archiveNames;

    bool create = false;
    bool list = false;
    bool extract = false;
    bool append = false;
    bool delete_ = false;
    bool concatenate = false;
    uint32_t decodeLen = 10;

    Parser(const int argc, char** argv) {
        for (uint32_t i = 1; i < argc; ++i) {
            std::string option = argv[i];
            if (option == "--create" || option == "-c") {
                create = true;
            } else if (option == "--file" || option == "-f") {
                archiveNames.emplace_back(argv[i + 1]);
                i++;
            } else if (option == "--list" || option == "-l") {
                list = true;
            } else if (option == "--extract" || option == "-x") {
                extract = true;
            } else if (option == "--append" || option == "-a") {
                append = true;
            } else if (option == "--delete" || option == "-d") {
                delete_ = true;
            } else if (option == "--concatenate" || option == "-A") {
                concatenate = true;
            } else if (option == "--decodelen" || option == "-dl") {
                std::cout << "its decode" << std::endl;
                decodeLen = std::stoi(argv[i + 1]);
                i++;
            } else {
                if (option.size() >= 5 && std::string(option, option.length() - 4, 4) == ".haf") {
                    archiveNames.emplace_back(option);
                } else {
                    fileNames.emplace_back(option);
                }
            }
        }
    }
};

#endif //LABWORK_4_EXC11G_PARSER_H


