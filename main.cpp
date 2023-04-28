#include <iostream>

#include "arch.h"
#include "parser.h"


int main(int argc, char** argv) {
    Parser parser(argc, argv);
    if (parser.create) {
        if (parser.archiveNames.empty()) {
            std::cout << "Archive not found.." << std::endl;
            exit(1);
        } else {
            CreateArchive(parser.archiveNames[0], parser.decodeLen);
            if (!parser.fileNames.empty()) {
                for (uint64_t i = 0; i < parser.fileNames.size(); ++i) {
                    AppendFile(parser.fileNames[i], parser.archiveNames[0], parser.decodeLen);
                }
            } else {
                std::cout << "Files not found.." << std::endl;
                exit(1);
            }
            std::cout << "Archive was created.." << std::endl;
        }
    } else if (parser.list) {
        std::ofstream nullFile("");
        DecodeArchive(parser.archiveNames[0], parser, nullFile);
    } else if (parser.extract) {
        if (parser.archiveNames.empty()) {
            std::cout << "Archive not found.." << std::endl;
            exit(1);
        } else {
            if (parser.fileNames.empty()) {
                std::cout << "Files not found" << std::endl;
                exit(1);
            } else {
                for (uint32_t i = 0; i < parser.fileNames.size(); ++i) {
                    std::ofstream file(parser.fileNames[i], std::ios::binary | std::ios::out);
                    DecodeArchive(parser.archiveNames[0], parser, file);
                }
                std::cout << "This files were extracted.." << std::endl;
            }
        }
    } else if (parser.append) {
        if (parser.archiveNames.empty()) {
            std::cout << "Archive not found.." << std::endl;
            exit(1);
        } else {
            if (parser.fileNames.empty()) {
                std::cout << "Files not found" << std::endl;
                exit(1);
            } else {
                for (uint32_t i = 0; i < parser.fileNames.size(); ++i) {
                    AppendFile(parser.fileNames[i], parser.archiveNames[0], parser.decodeLen);
                }
                std::cout << "Append passed successfully.." << std::endl;
            }
        }
    } else if (parser.delete_) {
        if (parser.archiveNames.empty()) {
            std::cout << "Archive not found.." << std::endl;
            exit(1);
        } else {
            if (parser.fileNames.empty()) {
                std::cout << "Files not found.." << std::endl;
                exit(1);
            } else {
                for (uint32_t i = 0; i < parser.fileNames.size(); ++i) {
                    DeleteFile(parser.archiveNames[0], parser.fileNames[i], parser);
                }
                std::cout << "Delete was successfully.." << std::endl;
            }
        }
    } else if (parser.concatenate) {
        if (parser.archiveNames.empty()) {
            std::cout << "Archives not found.." << std::endl;
            exit(1);
        } else {
            ConcatenateArchives(parser.archiveNames);
            std::cout << "Concatenate passed successfully.." << std::endl;
        }
    }

    return 0;
}