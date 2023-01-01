#include <d3d9.h>
#include <string>
#include "detours.h"
#include "framework.h"

#include "dxhook/mainHook.h"

using namespace GarrysMod::Lua;

GMOD_MODULE_OPEN()
{
	return DXHook::Initialize(LUA, false);
}

GMOD_MODULE_CLOSE()
{
	return DXHook::Cleanup(LUA);
}