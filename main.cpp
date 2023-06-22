#define GMMODULE

#include "Interface.h"

// Called when the module opens
GMOD_MODULE_OPEN()
{
    return 0;
}
// Called when the module closes
GMOD_MODULE_CLOSE()
{
    return 0;
}