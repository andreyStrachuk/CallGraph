#include <stdio.h>
#include <iostream>
#include <map>

std::map<std::pair<int64_t, int64_t>, int64_t> funcTable;
FILE *output;

extern "C"
void collectData (const int64_t callerAddr, const int64_t calleeAddr) {
    printf ("Addresses: %ld %ld\n", callerAddr, calleeAddr);
    std::pair<int64_t, int64_t> funcPair{callerAddr, calleeAddr};

    auto isFound = funcTable.find(funcPair);
    if (isFound == funcTable.end()) {
        funcTable[funcPair] = 1;
    }
    else {
        funcTable[funcPair] += 1;
    }
}

extern "C"
void openFile () {
    printf ("I am in open file!\n");
    output = fopen ("dump_dot.txt", "a");
}

extern "C"
void writeToFile () {
    printf ("I am in write to file!\n");

    for (auto &MO : funcTable) {
        fprintf (output, "%ld -> %ld [label = %ld]\n", MO.first.first, MO.first.second, MO.second);
    }

    fprintf (output, "}\n");

    fclose(output);
}