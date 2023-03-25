#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

#include "stringLib.h"

std::map <std::pair<u_int64_t, u_int64_t>, int> funcHashTable;
std::map <u_int64_t, char *> funcNames;
std::vector <u_int64_t> hashesInFile;

std::ofstream output;
FILE *input;

extern "C"
void collectData (const u_int64_t callerHash, const u_int64_t calleeHash) {
    std::pair<u_int64_t, u_int64_t> funcPair{callerHash, calleeHash};

    auto isFound = funcHashTable.find(funcPair);
    if (isFound == funcHashTable.end()) {
        funcHashTable[funcPair] = 1;
    }
    else {
        funcHashTable[funcPair] += 1;
    }
}

extern "C"
void openFile () {
    input = fopen("inter.txt", "r");
    if (!input) {
        std::cout << "Unable to open file!\n";
    }
}

static void inline putNameIntoFile (std::vector<u_int64_t> &hashesInFile, const u_int64_t hash) {
    hashesInFile.push_back(hash);

    char *line = funcNames[hash];

    output << line << "\n";
}

extern "C"
void writeToFile () {
    openFile();

    size_t fileSize = getFileSize (input);
    size_t numberOfStrings = getNumberOfStrings (input);

    char *text = bufferAlloc(&fileSize, input);
    if (!text)
        return;
    fclose (input);

    char **arrOfPtrs = new char*[numberOfStrings];
    initializeArrOfPointers (arrOfPtrs, numberOfStrings, text);

    for (int i = 0; i < numberOfStrings; i++) {
        u_int64_t hash = getHash(arrOfPtrs[i]);

        funcNames[hash] = arrOfPtrs[i];
    }

    output.open ("dump_dot.txt", std::ofstream::trunc);

    output << "digraph D {\n";

    for (auto &MO : funcHashTable) {

        u_int64_t callerHash = MO.first.first;
        u_int64_t calleeHash = MO.first.second;
        int numberOfCalls = MO.second;

        if (std::find(hashesInFile.begin(), hashesInFile.end(), callerHash) == hashesInFile.end()) {
            putNameIntoFile (hashesInFile, callerHash);
        }

        if (std::find(hashesInFile.begin(), hashesInFile.end(), calleeHash) == hashesInFile.end()) {
            putNameIntoFile (hashesInFile, calleeHash);
        }

        output << callerHash << " -> " << calleeHash << "[label = \"" << numberOfCalls << "\"];\n";
    }

    output << "}\n";
    
    delete[] arrOfPtrs;
    output.close();
}
