#include <stdio.h>
#include <chrono>
using namespace std;

#include "dccmtr.h"

int main(int argc, char **argv)
{
    int pX = 0, pY = 0, pZ = 0;

    if(!DC_Startup(0,0)) {
        printf("could not open sensor\n");
        return -1;
    }
    printf("opened sensor\n");
    printf("               X        Y        Z    \n");
    for (int i = 0; i < 3; i++) {
        auto t1 = chrono::high_resolution_clock::now();
        int status = DC_GetXYZ(1, &pX, &pY, &pZ);
        auto t2 = chrono::high_resolution_clock::now();
        printf("status %d, xyz: %-8.3f %-8.3f %-8.3f, took %lld ms\n", status, pX / 1000.0, pY / 1000.0, pZ / 1000.0, chrono::duration_cast<chrono::milliseconds>(t2 - t1).count());;
    }
    DC_Shutdown();
    printf("finished\n");
    getchar();
    return 0;
}