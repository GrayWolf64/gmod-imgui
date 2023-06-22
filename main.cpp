#define GMMODULE

#include "Interface.h"
#include "kiero.h"
#include "d3d9.h"
#include "Windows.h"
#include "minhook/include/MinHook.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx9.h"

// Called when the module opens
GMOD_MODULE_OPEN()
{
	long long pid = GetCurrentProcessId();
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->GetField(-1, "print");
    LUA->PushNumber(pid);
    LUA->Call(1, 0);
    LUA->Pop();
	return 0;
}

// Called when the module closes
GMOD_MODULE_CLOSE()
{
    return 0;
}