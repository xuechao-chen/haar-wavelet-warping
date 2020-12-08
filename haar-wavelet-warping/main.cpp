#include "WarpingAppOnCPU.h"

int main()
{
    LAUNCH_APPLICATION(512, 512, WarpingAppOnCPU);

    return 0;
}