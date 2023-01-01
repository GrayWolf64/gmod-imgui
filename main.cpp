#include <d3d9.h>
#include <string>
#include "detours.h"
#include "framework.h"

#include "dxhook/mainHook.h"

using namespace GarrysMod::Lua;

// Called when the module is loaded
GMOD_MODULE_OPEN()
{
	return DXHook::Initialize(LUA, false);
}

// Called when the module is unloaded
GMOD_MODULE_CLOSE()
{
	return DXHook::Cleanup(LUA);
}