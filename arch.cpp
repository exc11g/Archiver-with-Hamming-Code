#include <iostream>
#include <vector>
#include <cmath>

#include "arch.h"
#include "archiver.h"
#include "parser.h"

const uint8_t kLenOfCodeArchive = 9;
const uint8_t kByte = 8;


bool IsPowerOfTwo(uint64_t num) {

    return num && !(num & (num - 1));
}

uint64_t CountOfControlBits(uint64_t countOfBits) {
    uint64_t controlBits = 1;
    while ((uint64_t) pow(2, controlBits) < controlBits + countOfBits + 1) {
        ++controlBits;
    }

    return controlBits;
}

void CodeFileHeader(std::ofstream& archive, const std::string& fileName, const uint64_t sizeFile, const uint64_t hammingLen) {
    char buff[CountOfDigs(sizeFile)];
    itoa(sizeFile, buff, 10);
    std::string fileHeader = "<" + fileName + ":" + buff + ">";
    std::vector<bool> hammingSequence;
    std::vector<bool> temp;
    for (uint64_t i = 0; i < fileHeader.size(); ++i) {
        std::vector<bool> charToBin = NumToBin(fileHeader[i]);
        for (uint8_t j = 0; j < kByte; ++j) {
            temp.push_back(charToBin[j]);
        }
        while (temp.size() >= hammingLen) {
            for (uint64_t j = 0; j < hammingLen; ++j) {
                if (IsPowerOfTwo(j + 1)) {
                    hammingSequence.push_back(false);
                }
                hammingSequence.push_back(temp[j]);
            }
            CalculationControlBits(hammingSequence);
            while (hammingSequence.size() >= kByte) {
                archive << BinToChar(std::vector<bool>(hammingSequence.begin(), hammingSequence.begin() + 8));
                hammingSequence = std::vector<bool>(hammingSequence.begin() + kByte, hammingSequence.end());
            }
            temp = std::vector<bool>(temp.begin() + kByte, temp.end());
        }
    }
    while (temp.size() < hammingLen) {
        temp.push_back(false);
    }
    CalculationControlBits(temp);
    for (uint64_t i = 0; i < temp.size(); ++i) {
        hammingSequence.push_back(temp[i]);
    }
    if (!hammingSequence.empty()) {
        for (uint64_t i = 0; i < temp.size(); ++i) {
            hammingSequence.push_back(temp[i]);
        }
        uint64_t hamSize = hammingSequence.size();
        for (uint64_t i = 0; i < kByte - hamSize % kByte; ++i) {
            hammingSequence.push_back(false);
        }
        while (hammingSequence.size() >= kByte) {
            archive << BinToChar(std::vector<bool>(hammingSequence.begin(), hammingSequence.begin() + kByte));
            hammingSequence = std::vector<bool>(hammingSequence.begin() + kByte, hammingSequence.end());
        }
    }
}

uint8_t BinToChar(const std::vector<bool>& a) {
    uint8_t ch = 0;
    for (uint8_t i = kByte; i > 0; --i) {
        if (a[kByte -  i]) {
            ch += pow(2, i - 1);
        }
    }

    return ch;
}


void PrintVector(std::vector<int>& a) {
    for (uint64_t i = 0; i < a.size(); ++i) {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;
}

uint32_t DecodeHammingLen(std::ifstream& archive) {
    char codedHammingLen[kLenOfCodeArchive];
    archive.read(codedHammingLen, kLenOfCodeArchive);
    std::vector<bool> temp;
    uint64_t errorCount = 0;
    for (uint8_t i = 0; i < kLenOfCodeArchive; ++i) {
        std::vector<bool> charToBin = NumToBin(codedHammingLen[i]);
        for (uint8_t j = 0; j < kByte; ++j) {
            temp.push_back(charToBin[j]);
        }
    }
    uint8_t i = 0;
    temp = DecodeSequence(temp);

    char decodedHammingLen[kByte];
    char ch;
    while (temp.size() >= kByte) {
        ch = BinToChar(std::vector<bool>(temp.begin(), temp.begin() + kByte));
        temp.erase(temp.begin(), temp.cbegin() + kByte);
        decodedHammingLen[i++] = ch;
    }
    uint32_t hammingLen = std::stoi(decodedHammingLen);

    return hammingLen;
}

std::vector<std::string> DecodeFileHeader(std::ifstream& archive) {
    uint32_t hammingLen = DecodeHammingLen(archive);
    std::vector<bool> temp;
    std::vector<bool> decodedSequence;
    char ch;
    std::vector<char> decodedHeader;
    while (archive.get(ch)) {
        std::vector<bool> charToBin = NumToBin(ch);
        for (uint8_t i = 0; i < kByte; ++i) {
            temp.push_back(charToBin[i]);
        }
        while (temp.size() >= hammingLen + CountOfControlBits(hammingLen)) {
            temp = DecodeSequence(temp);
            for (uint32_t j = 0; j < hammingLen; ++j) {
                decodedSequence.push_back(temp[j]);
            }
        }
        char decodeChar;
        while (decodedSequence.size() >= kByte) {
            decodeChar = BinToChar(std::vector<bool>(decodedSequence.begin(), decodedSequence.begin() + kByte));
            decodedSequence.erase(decodedSequence.begin(), decodedSequence.begin() + kByte);
            if (decodeChar == '>') {
                break;
            }
            decodedHeader.push_back(decodeChar);
        }
    }
    std::vector<std::string> infoFile(2);
    for (uint32_t i = 0; i < decodedHeader.size(); ++i) {
        if (decodedHeader[i] == ':') {
            infoFile[0] = std::string(decodedHeader.begin(), decodedHeader.begin() + i + 1);
            infoFile[1] = std::string(decodedHeader.begin() + i + 2, decodedHeader.end());
        }
    }
    char buff[CountOfDigs(hammingLen)];
    itoa(hammingLen, buff, 10);
    infoFile[2] = buff;

    return infoFile;
}

void DecodeArchive(const std::string& archiveName, Parser& parser, std::ofstream& file) {
    std::ifstream archive(archiveName + ".haf", std::ios::binary | std::ios::in);
    std::vector<std::string> infoFile = DecodeFileHeader(archive);
    std::vector<bool> temp;
    std::vector<bool> hammingSequence;
    std::vector<std::string> fileNames;
    char ch;
    uint32_t hammingLen = std::stoi(infoFile[2]);
    uint64_t sizeFile = std::stoi(infoFile[1]);
    bool stopFlag = false;
    while (archive.get(ch) && !stopFlag) {
        std::vector<bool> charToBin = NumToBin(ch);
        for (uint8_t i = 0; i < kByte; ++i) {
            temp.push_back(charToBin[i]);
        }
        uint32_t allHammingLen = hammingLen + CountOfControlBits(hammingLen);
        while (temp.size() >= allHammingLen && !stopFlag) {
            for (uint32_t i = 0; i < allHammingLen; ++i) {
                hammingSequence.push_back(temp[i]);
            }
            hammingSequence = DecodeSequence(hammingSequence);
            while (hammingSequence.size() >= kByte) {
                --sizeFile;
                if (parser.extract) {
                    file << BinToChar(std::vector<bool>(hammingSequence.begin(), hammingSequence.begin() + kByte));
                    hammingSequence.erase(hammingSequence.begin(), hammingSequence.begin() + kByte);
                }
                if (sizeFile == 0) {
                    infoFile = DecodeFileHeader(archive);
                }
                if (sizeFile == 0 && parser.extract) {
                    stopFlag = true;
                    break;
                } else if (sizeFile == 0 && parser.list) {
                    fileNames.emplace_back(infoFile[0]);
                    sizeFile = std::stoi(infoFile[1]);
                    hammingLen = std::stoi(infoFile[2]);
                    uint32_t codedSize;
                    if ((sizeFile * 8 + CountOfControlBits(sizeFile * 8)) % 8 == 0) {
                        codedSize = (sizeFile * 8 + CountOfControlBits(sizeFile * 8)) / 8;
                    } else {
                        codedSize = (sizeFile * 8 + CountOfControlBits(sizeFile * 8)) / 8 + 1;
                    }
                    archive.seekg(archive.tellg() + (std::streampos)codedSize);
                }
            }
            temp = std::vector<bool>(temp.begin() + allHammingLen, temp.end());
        }
    }
}

void DeleteFile(const std::string& archiveName, const std::string& fileName, Parser& parser) {
    std::ofstream tempFile("temp", std::ios::binary | std::ios::out);
    std::ifstream archive(archiveName, std::ios::binary | std::ios::in);
    std::vector<std::string> infoFile = DecodeFileHeader(archive);
    std::vector<bool> temp;
    std::vector<bool> hammingSequence;
    std::vector<std::string> fileNames;
    char ch;
    uint32_t hammingLen = std::stoi(infoFile[2]);
    uint64_t sizeFile = std::stoi(infoFile[1]);
    bool stopFlag = false;
    while (archive.get(ch) && !stopFlag) {
        std::vector<bool> charToBin = NumToBin(ch);
        for (uint8_t i = 0; i < kByte; ++i) {
            temp.push_back(charToBin[i]);
        }
        uint32_t allHammingLen = hammingLen + CountOfControlBits(hammingLen);
        while (temp.size() >= allHammingLen && !stopFlag) {
            for (uint32_t i = 0; i < allHammingLen; ++i) {
                hammingSequence.push_back(temp[i]);
            }
            hammingSequence = DecodeSequence(hammingSequence);
            while (hammingSequence.size() >= kByte) {
                --sizeFile;
                if (parser.fileNames[0] == fileName) {
                    tempFile << BinToChar(std::vector<bool>(hammingSequence.begin(), hammingSequence.begin() + kByte));
                    hammingSequence.erase(hammingSequence.begin(), hammingSequence.begin() + kByte);
                }
                if (sizeFile == 0) {
                    infoFile = DecodeFileHeader(archive);
                }
                if (sizeFile == 0 && parser.extract) {
                    stopFlag = true;
                    break;
                } else if (sizeFile == 0 && parser.list) {
                    fileNames.emplace_back(infoFile[0]);
                    sizeFile = std::stoi(infoFile[1]);
                    hammingLen = std::stoi(infoFile[2]);
                }
            }
            temp = std::vector<bool>(temp.begin() + allHammingLen, temp.end());
        }
    }
    archive.close();
    std::ofstream newArch(archiveName, std::ios::binary | std::ios::out);
    std::ifstream tempArch("temp", std::ios::binary | std::ios::in);
    while (tempArch.get(ch)) {
        newArch << ch;
    }
}

void ConcatenateArchives(const std::vector<std::string>& archiveNames) {
    std::ofstream mainArchive(archiveNames[0], std::ios::binary | std::ios::out | std::ios::app);
    for (uint64_t i = 1; i < archiveNames.size(); ++i) {
        std::ifstream tempArchive(archiveNames[i], std::ios::binary | std::ios::in);
        char ch;
        while (tempArchive.get(ch)) {
            mainArchive << ch;
        }
    }
}

void CalculationControlBits(std::vector<bool>& hammingSequence) {
    bool bitSum;
    for (uint64_t i = 1; i < hammingSequence.size(); i = i * 2) {
        bitSum = false;
        uint64_t count = 0;
        uint64_t j = i - 1;
        while (j < hammingSequence.size()) {
            bitSum = bitSum != hammingSequence[j];
            count++;
            if (count == i) {
                j += i + 1;
                count = 0;
            } else {
                ++j;
            }
        }
        hammingSequence[i - 1] = bitSum;
    }
}

uint16_t CountOfDigs(uint64_t num) {
    uint16_t result = 0;
    while (num != 0) {
        ++result;
        num /= 10;
    }

    return result;
}

std::vector<bool> NumToBin(uint8_t num) {
    uint8_t k = 7;
    std::vector<bool> result(kByte);
    result.assign(8, false);
    while (num > 0) {
        if (pow(2, k) <= num) {
            result[kByte - k - 1] = true;
            num -= (uint8_t) pow(2, k);
        }
        --k;
    }

    return result;
}

std::vector<bool> AppendFile(const std::string& fileName, const std::string& archiveName, const uint64_t hammingLen) {

    std::ifstream file(fileName, std::ios::binary | std::ios::in);
    std::ofstream outFile(archiveName + ".haf", std::ios::binary | std::ios::out | std::ios::app);
    file.seekg(std::ios::end);
    std::vector<bool> temp;
    std::vector<bool> hammingSequence;
    uint64_t sizeFile = file.tellg();
    CodeFileHeader(outFile, fileName, sizeFile, hammingLen);
    std::cout << sizeFile << std::endl;
    file.seekg(0);
    char ch;
    while (file.get(ch)) {
        std::vector<bool> charToBin = NumToBin(ch);
        for (uint8_t i = 0; i < 8; ++i) {
            temp.push_back(charToBin[i]);
        }
        while (temp.size() >= hammingLen) {
            for (uint64_t i = 0; i < hammingLen; ++i) {
                if (IsPowerOfTwo(i + 1)) {
                    hammingSequence.push_back(false);
                }
                hammingSequence.push_back(temp[i]);
            }
            CalculationControlBits(hammingSequence);
            while (hammingSequence.size() >= kByte) {
                outFile << BinToChar(std::vector<bool>(hammingSequence.begin(), hammingSequence.begin() + kByte));
                hammingSequence.erase(hammingSequence.begin(), hammingSequence.begin() + kByte);
            }
            temp.erase(temp.begin(), temp.begin() + kByte);
        }
    }
    while (temp.size() < hammingLen) {
        temp.push_back(false);
    }
    CalculationControlBits(temp);
    for (uint64_t i = 0; i < temp.size(); ++i) {
        hammingSequence.push_back(temp[i]);
    }
    if (!hammingSequence.empty()) {
        for (uint64_t i = 0; i < temp.size(); ++i) {
            hammingSequence.push_back(temp[i]);
        }
        uint64_t hamSize = hammingSequence.size();
        for (uint64_t i = 0; i < kByte - hamSize % kByte; ++i) {
            hammingSequence.push_back(false);
        }
        while (hammingSequence.size() >= kByte) {
            outFile << BinToChar(std::vector<bool>(hammingSequence.begin(), hammingSequence.begin() + kByte));
            hammingSequence.erase(hammingSequence.begin(), hammingSequence.begin() + kByte);
        }
    }
    outFile.close();

    return temp;
}

bool fixSequence(std::vector<bool>& hammingSequence) {
    uint32_t sindromPos = 0;
    bool bitSum = false;

    for (uint64_t i = 1; i < hammingSequence.size(); i *= 2) {
        bitSum = false;
        uint64_t count = 0;
        uint64_t j = i - 1;
        while (j < hammingSequence.size()) {
            bitSum = bitSum != hammingSequence[j];
            count++;
            if (count == i) {
                j += i + 1;
                count = 0;
            } else {
                ++j;
            }
        }
        sindromPos += bitSum * i;
    }
    if (sindromPos != 0) {
        hammingSequence[sindromPos - 1] = !hammingSequence[sindromPos - 1];
        std::cout << "Error was fixed.." << std::endl;
        return true;
    }
    return false;
}

std::vector<bool> DecodeSequence(std::vector<bool>& hammingSequence) {
    fixSequence(hammingSequence);
    if (fixSequence(hammingSequence)) {
        std::cout << "The error cannot be corrected.." << std::endl;
    }
    std::vector<bool> decodedSequence;
    for (uint64_t i = 0; i < hammingSequence.size(); ++i) {
        if (!IsPowerOfTwo(i + 1)) {
            decodedSequence.push_back(hammingSequence[i]);
        }
    }

    return decodedSequence;
}

std::vector<bool> CodeSequence(const std::vector<bool>& bits) {
    std::vector<bool> codedSequence;
    for (uint64_t i = 0; i < bits.size(); ++i) {
        if (IsPowerOfTwo(i + 1)) {
            codedSequence.push_back(false);
        }
        codedSequence.push_back(bits[i]);
    }
    CalculationControlBits(codedSequence);

    return codedSequence;
}

void UnHamming(const std::string& fileName, uint64_t hammingLen) {
    std::ifstream file(fileName, std::ios::binary | std::ios::in);
    std::ofstream outFile("decodefile.txt", std::ios::binary | std::ios::out);
    std::vector<bool> hammingSequence;
    char ch;
    uint64_t countOfControlBits = CountOfControlBits(hammingLen);
    while (file.get(ch)) {
        std::vector<bool> charToBin = NumToBin(ch);
        for (uint8_t i = 0; i < kByte; ++i) {
            hammingSequence.push_back(charToBin[i]);
        }
        while (hammingSequence.size() >= countOfControlBits + hammingLen) {
            std::vector<bool> subSequence = std::vector<bool>(hammingSequence.begin(),
                                                              hammingSequence.begin() + hammingLen + countOfControlBits);
            std::vector<bool> decodedSequence = DecodeSequence(subSequence);
            while (decodedSequence.size() >= kByte) {
                outFile << (unsigned char)BinToChar(std::vector<bool>(decodedSequence.begin(), decodedSequence.begin() + kByte));
            }
        }
    }
}

void CreateArchive(const std::string& nameOfArchive, const uint32_t decodeLen) {
    std::ofstream archive(nameOfArchive + ".haf", std::ios::binary | std::ios::out);
    char buff[kByte];
    itoa(decodeLen, buff, 10);
    std::vector<bool> binBuff;
    for (uint8_t i = 0; i < kByte; ++i) {
        std::vector<bool> temp = NumToBin(buff[i]);
        for (uint8_t j = 0; j < kByte; ++j) {
            binBuff.push_back(temp[j]);
        }
    }
    std::vector<bool> codedLenOfCode = CodeSequence(binBuff);
    codedLenOfCode.push_back(false); // т.к.  было 71 бит дополним еще битиком до 72
    while (codedLenOfCode.size() >= kByte) {
        archive << BinToChar(std::vector<bool>(codedLenOfCode.begin(), codedLenOfCode.begin() + kByte));
        codedLenOfCode.erase(codedLenOfCode.begin(), codedLenOfCode.begin() + kByte);
    }
}