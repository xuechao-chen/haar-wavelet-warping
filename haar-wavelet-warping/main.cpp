#include "WarpingAppOnGPUByLayer.h"

int main()
{
    LAUNCH_APPLICATION(256,256,WarpingAppOnGPUByLayer);

    return 0;
}