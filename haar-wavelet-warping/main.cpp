#include "WarpingAppOnCPU.h"
#include "WarpingAppOnGPU1.h"

int main()
{
    //LAUNCH_APPLICATION(512, 512, WarpingAppOnCPU);
    LAUNCH_APPLICATION(256,256,WarpingAppOnGPU1);

    return 0;
}