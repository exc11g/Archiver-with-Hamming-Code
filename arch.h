#include <string>
#include <vector>
#include <fstream>
#include <charconv>
#include <iostream>

#include "parser.h"

#ifndef LABWORK_4_EXC11G_ARCH_H
#define LABWORK_4_EXC11G_ARCH_H

void CreateArchive(const std::string& nameOfArchive, const uint32_t decodeLen);

std::vector<bool> AppendFile(const std::string& fileName, const std::string& archiveName, const uint64_t hammingLen);

void CodeFileHeader(std::ofstream& archive, const std::string& fileName, const uint64_t sizeFile, const uint64_t hammingLen);

uint16_t CountOfDigs(uint64_t num);

bool IsPowerOfTwo(uint64_t num);

uint64_t CountOfControlBits(uint64_t countOfBits);

uint8_t BinToChar(const std::vector<bool>& a);

void PrintVector(std::vector<int>& a);

void ConcatenateArchives(const std::vector<std::string>& archiveNames);

void DeleteFile(const std::string& archiveName, const std::string& fileName, Parser& parser);

std::vector<std::string> DecodeFileHeader(std::ifstream& archive);

uint32_t DecodeHammingLen(std::ifstream& archive);

void DecodeArchive(const std::string& archiveName, Parser& parser, std::ofstream& file);

void CalculationControlBits(std::vector<bool>& hammingSequence);

std::vector<bool> NumToBin(uint8_t num);

std::vector<bool> DecodeSequence(std::vector<bool>& hammingSequence);

bool fixSequence(std::vector<bool>& hammingSequence);

void UnHamming(const std::string& fileName, const uint64_t hammingLen);

#endif //LABWORK_4_EXC11G_ARCH_H
