#include <stdio.h>

void collectData (__int64_t callerAddr, __int64_t calleeAddr) {
    printf ("Addresses: %ld %ld\n", callerAddr, calleeAddr);
}
