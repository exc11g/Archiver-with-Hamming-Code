#include <string>
#include <vector>
#include <fstream>
#include <charconv>
#include <iostream>
#include <cstdio>


#ifndef LABWORK_4_EXC11G_ARCHIVER_H
#define LABWORK_4_EXC11G_ARCHIVER_H

const uint8_t MAX_SIZE_OF_INFO = 8;

std::vector<std::pair<std::string, uint64_t>> getFilesInfo(const std::string& pathToArchive, const std::string& nameOfArchive) {
    std::vector<std::pair<std::string, uint64_t>> result;
    std::ifstream archive(pathToArchive + nameOfArchive + ".haf", std::ios::binary | std::ios::in);
    char buff[MAX_SIZE_OF_INFO];
    archive.read(buff, MAX_SIZE_OF_INFO);
    const uint32_t sizeOfInfo = std::stoi(buff);
    char infoFiles[sizeOfInfo];
    archive.read(infoFiles, sizeOfInfo);
    std::string infoFilesAsString = std::string(infoFiles);
    uint32_t i = 1;

    while (i < sizeOfInfo) {
        uint32_t tempPos = i;
        while (infoFiles[i] != '>') {
            ++i;
        }
        std::string nameOfFile = std::string(infoFilesAsString, tempPos, i - 1 - tempPos + 1);
        tempPos = i + 1;
        while (infoFiles[i] != '|') {
            ++i;
        }
        uint64_t sizeOfFile = stoi(std::string(infoFilesAsString, tempPos, i - 1 - tempPos + 1));
        result.emplace_back(std::make_pair(nameOfFile, sizeOfFile));
        ++i;
    }
    return result;
}

void PushFilesToArchive(const std::string& filesInfo, const std::vector<std::string>& fileNames,
                        const std::string& pathOfArchive, const std::string& nameOfArchive) {
    std::ofstream archive(pathOfArchive + "/" + nameOfArchive + ".haf", std::ios::binary | std::ios::out);
    archive << filesInfo;
    char buff;
    for (uint16_t i = 0; i < fileNames.size(); ++i) {
        std::ifstream file(fileNames[i], std::ios::binary | std::ios::in);
        while (file) {
            if (file.get(buff)) {
                archive << buff;
            } else {
                break;
            }
        }
    }
}

std::string getNameInPath(const std::string& path) {
    uint64_t pos = path.rfind('/');
    return std::string(path, pos + 1, path.size() - pos + 1);
}

static uint16_t CountOfDigs(uint64_t num) {
    uint16_t result = 0;
    while (num != 0) {
        ++result;
        num /= 10;
    }

    return result;
}

std::string ParseInfo(const std::vector<std::string>& fileNames) {
    std::string filesInfo = "|";
    uint64_t sizeOfInfo = 1;
    uint64_t sizeOfFile;
    uint8_t countDigs;

    for (uint32_t i = 0; i < fileNames.size(); ++i) {
        std::ifstream file(fileNames[i], std::ios::in | std::ifstream::ate | std::ifstream::binary);
        std::string nameOfFile = getNameInPath(fileNames[i]);
        file.seekg(0, std::ios::end);
        sizeOfFile = file.tellg();
        filesInfo += nameOfFile;
        filesInfo += ">";
        sizeOfInfo += 1 + nameOfFile.size();
        countDigs = CountOfDigs(sizeOfFile);
        char* buff = new char[countDigs];
        itoa(sizeOfFile, buff, 10);
        filesInfo += buff;
        filesInfo += "|";
        sizeOfInfo += 1 + countDigs;

        delete[] buff;
    }
    char buff[MAX_SIZE_OF_INFO];
    for (int i = MAX_SIZE_OF_INFO - 1; i >= 0; --i) {
        if (sizeOfInfo != 0) {
            buff[i] = (char)(sizeOfInfo % 10 + 48);
            sizeOfInfo /= 10;
        } else {
            buff[i] = '0';
        }
    }
    filesInfo.insert(0, buff);

    return filesInfo;
}

void UnArchiver(const std::string& archiveName) {
    std::ifstream archive(archiveName + ".haf", std::ios::binary | std::ios::in);
    char buff[MAX_SIZE_OF_INFO];
    archive.read(buff, MAX_SIZE_OF_INFO);
    const uint32_t sizeOfInfo = std::stoi(buff);
    char infoFiles[sizeOfInfo];
    archive.read(infoFiles, sizeOfInfo);
    std::string infoFilesAsString = std::string(infoFiles);
    uint32_t i = 1;

    while (i < sizeOfInfo) {
        uint32_t tempPos = i;
        while (infoFiles[i] != '>') {
            ++i;
        }
        std::string nameOfFile = std::string(infoFilesAsString, tempPos, i - 1 - tempPos + 1);
        tempPos = i + 1;
        while (infoFiles[i] != '|') {
            ++i;
        }
        uint64_t sizeOfFile = stoi(std::string(infoFilesAsString, tempPos, i - 1 - tempPos + 1));
        ++i;

        std::ofstream outFile(nameOfFile, std::ios::binary | std::ios::out);
        char ch;
        for (uint32_t i = 0; i < sizeOfFile; ++i) {
            if (archive.get(ch)) {
                outFile << ch;
            }
        }
        outFile.close();
    }
}

void createArchive(const std::string& nameOfArchive, const std::vector<std::string>& fileNames,
                   const std::string& pathOfArchive) {
    std::string filesInfo = ParseInfo(fileNames);
    PushFilesToArchive(filesInfo, fileNames, pathOfArchive, nameOfArchive);
}

void printListOfFiles(const std::string& pathToArchive, const std::string& nameOfArchive) {
    std::ifstream archive(pathToArchive + nameOfArchive + ".haf", std::ios::binary | std::ios::in);
    char buff[MAX_SIZE_OF_INFO];
    archive.read(buff, MAX_SIZE_OF_INFO);

    const uint32_t sizeOfInfo = std::stoi(buff);
    char infoFiles[sizeOfInfo];
    archive.read(infoFiles, sizeOfInfo);
    std::string infoFilesAsString = std::string(infoFiles);
    uint32_t i = 1;
    while (i < sizeOfInfo) {
        uint32_t tempPos = i;
        while (infoFiles[i] != '>') {
            ++i;
        }
        std::cout << std::string(infoFilesAsString, tempPos, i - 1 - tempPos + 1) << " weight = ";
        tempPos = i + 1;
        while (infoFiles[i] != '|') {
            ++i;
        }
        std::cout << stoi(std::string(infoFilesAsString, tempPos, i - 1 - tempPos + 1)) << std::endl;
        ++i;
    }
}

void extractFile(const std::string& pathToArchive, const std::string& nameOfArchive,
                  const std::string& nameOfExtractFile, bool flagAllFiles) {
    std::ifstream archive(pathToArchive + nameOfArchive + ".haf", std::ios::binary | std::ios::in);
    char buff[MAX_SIZE_OF_INFO];
    archive.read(buff, MAX_SIZE_OF_INFO);
    const uint32_t sizeOfInfo = std::stoi(buff);

    std::vector<std::pair<std::string, uint64_t>> filesInfo = getFilesInfo(pathToArchive, nameOfArchive);

    uint64_t sumOfBytes = MAX_SIZE_OF_INFO + sizeOfInfo;
    archive.seekg(sumOfBytes);
    for (uint64_t i = 0; i < filesInfo.size(); ++i) {
        if ((filesInfo[i].first == nameOfExtractFile) && flagAllFiles == 0) {
            std::ofstream outFile(pathToArchive + nameOfExtractFile, std::ios::binary | std::ios::out);
            archive.seekg(sumOfBytes);
            char ch;
            for (uint64_t j = 0; j < filesInfo[i].second; ++j) {
                if (archive.get(ch)) {
                    outFile << ch;
                }
            }
            sumOfBytes += filesInfo[i].second;
        } else if (flagAllFiles) {
            std::ofstream outFile(pathToArchive + filesInfo[i].first, std::ios::binary | std::ios::out);
            char ch;
            for (uint64_t j = 0; j < filesInfo[i].second; ++j) {
                if (archive.get(ch)) {
                    outFile << ch;
                }
            }
        }
        sumOfBytes += filesInfo[i].second;
    }
}

void deleteFile(const std::string& path, const std::string& nameOfArchive, const std::string& nameOfDeleteFile) {
    std::ifstream archive(path + nameOfArchive + ".haf", std::ios::binary | std::ios::in);
    std::ofstream tempArchive(path + "tmp.haf", std::ios::binary | std::ios::out);
    char buff[MAX_SIZE_OF_INFO];
    archive.read(buff, MAX_SIZE_OF_INFO);
    const uint32_t sizeOfInfo = std::stoi(buff);
    char infoFiles[sizeOfInfo];
    std::string infoFilesAsString = std::string(infoFiles);
    archive.read(infoFiles, sizeOfInfo);
    std::vector<std::pair<std::string, uint64_t>> filesInfo = getFilesInfo(path, nameOfArchive);
    uint64_t i = 1;
    while (i < sizeOfInfo) {
        uint32_t tempPos = i;
        while (infoFiles[i] != '>') {
            ++i;
        }
        std::string fileName = std::string(infoFilesAsString, tempPos, i - 1 - tempPos + 1);
        while (infoFiles[i] != '|') {
            ++i;
        }
        ++i;
        if (fileName == nameOfDeleteFile) {
            tempArchive << std::string(infoFilesAsString, tempPos, i - 1);
            break;
        }
    }

    char ch;
    for (uint64_t i = 0; i < filesInfo.size(); ++i) {
        if (filesInfo[i].first == nameOfDeleteFile) {
            archive.seekg(archive.tellg() + (std::streampos)filesInfo[i].second);
        }
        if (archive.get(ch)) {
            tempArchive << ch;
        }
    }
    std::string fullPath = path + nameOfArchive + ".haf";
    if (remove(fullPath)) {
        

    };
    rename()
}


#endif //LABWORK_4_EXC11G_ARCHIVER_H
