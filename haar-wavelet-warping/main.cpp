#include "WarpingAppOnCPU.h"

int main()
{
    std::shared_ptr<GLApp> pApp = std::make_shared<WarpingAppOnCPU>(512u, 512u, "WarpingAppOnCPU");
    pApp->run();

    return 0;
}