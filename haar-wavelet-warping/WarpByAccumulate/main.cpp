#include "WarpingAppOnGPUByAccumulate.h"

int main()
{
    LAUNCH_APPLICATION(256,256, WarpingAppOnGPUByAccumulate);

    return 0;
}