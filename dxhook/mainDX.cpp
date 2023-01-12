#include "../glua/Interface.h"
#include "mainHook.h"
#include <iostream>
#include <sstream>
#include "../detours.h"

#pragma comment(lib, "detours.lib")

#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    /* currently need a way to delegate input to ImGui whilest handling it accordingly to gmod aswell
    if (ImGui::GetIO().WantCaptureKeyboard || ImGui::GetIO().WantCaptureMouse)
        return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    */

    ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
    ImGuiIO &io = ImGui::GetIO();

    if (io.WantCaptureKeyboard && io.WantCaptureMouse)
    {
        DefWindowProc(hWnd, msg, wParam, lParam);

        return true; // block the input from reaching the gmod window
    }

    return CallWindowProc((WNDPROC)DXHook::originalWNDPROC, hWnd, msg, wParam, lParam);
}

namespace DXHook
{
    LONG_PTR originalWNDPROC;

    inline void error(GarrysMod::Lua::ILuaBase *LUA, const char *str)
    {
        LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->GetField(-1, "print");
        LUA->PushString(str);

        LUA->Call(1, 0);
        LUA->Pop();
    }

    int Initialize(GarrysMod::Lua::ILuaBase *LUA, bool OpenExConsole)
    {
        // used for setting up dummy device, and endscene hook

        // setup key codes
        // 0-9 key codes
        for (int i = 0x30; i <= 0x39; i++)
        {
            keyCodes.push_back(i);
        }

        // A-Z key codes
        for (int i = 0x41; i <= 0x5A; i++)
        {
            keyCodes.push_back(i);
        }

        if (OpenExConsole)
        {
            AllocConsole();
            FILE *pFile = nullptr;
            freopen_s(&pFile, "CONOUT$", "w", stdout); // cursed way to redirect stdout to our own console
        }

        HMODULE hDLL;
        hDLL = GetModuleHandleA("d3d9.dll"); // attempt to locate the d3d9 dll that gmod loaded

        if (hDLL == NULL)
        {
            error(LUA, "Unable to locate d3d9.dll in the loaded memory");
            return 0;
        }

        if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
        {
            // hook stuff using the dumped addresses

            using namespace std;
            stringstream hexLoc;
            hexLoc.setf(ios_base::hex, ios_base::basefield);

            hexLoc << string("EndScene functions address: ") << (uintptr_t)d3d9Device[42];

            error(LUA, hexLoc.str().c_str()); // it's called "error" but its more of a print

            oldFunc = (EndScene)d3d9Device[42]; // cast our void* address to a function description btw 42 is the EndScene index

            DetourTransactionBegin(); // use MS detours to detour our EndScene
            DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID &)oldFunc, EndSceneHook); // if im gon be straight honest i took this from guidedhacking and I don't entirely get
            // the "&(PVOID&)"

            LONG lError = DetourTransactionCommit(); // execute it
            if (lError != NO_ERROR)
            {
                MessageBox(HWND_DESKTOP, L"Failed to detour", L"puffy", MB_OK);
                return FALSE;
            }
            else
            {
                error(LUA, "Successfully got EndScene address, converted to function--detoured and ready");
            }
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO(); // (void)io;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        RECT resolutionDetails;
        HWND gamehwnd = GetProcessWindow();
        GetClientRect(gamehwnd, &resolutionDetails);

        ImGui::StyleColorsClassic();

        ImGui_ImplWin32_Init(gamehwnd);

        using namespace std;
        static bool isFullScreen;

        int WndWide = (resolutionDetails.right) - (resolutionDetails.left);
        int WndHeight = (resolutionDetails.bottom) - (resolutionDetails.top);

        LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->GetField(-1, "ScrW");
        LUA->Call(0, 1);
        int ScrW = (int)LUA->GetNumber(-1);
        LUA->Pop();
        LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->GetField(-1, "ScrH");
        LUA->Call(0, 1);
        int ScrH = (int)LUA->GetNumber(-1);
        LUA->Pop();

        int ScrX = GetSystemMetrics(SM_CXSCREEN);
        int ScrY = GetSystemMetrics(SM_CYSCREEN);

        if (ScrX == WndWide && ScrY == WndHeight)
        {
            isFullScreen = true;
        }
        else
        {
            isFullScreen = false;
        }

        // ImGui::GetMainViewport()->WorkSize = ImVec2(WndWide, WndHeight);

        if (OpenExConsole)
        {
            cout << "Client Rect W: " << WndWide << " H: " << WndHeight << endl;
            cout << "Client Game Res Settings (Option) W: " << ScrW << " H: " << ScrH << endl;
            cout << "System Screen X: " << ScrX << " Y: " << ScrY << endl;
            cout << "isFullScreen: " << isFullScreen << endl;
        }

        // io.DisplaySize.x = ScrX;
        // io.DisplaySize.y = ScrY;

        LONG_PTR currentWndProc = GetWindowLongPtr(gamehwnd, GWLP_WNDPROC);
        originalWNDPROC = currentWndProc;

        SetWindowLongPtr(gamehwnd, GWLP_WNDPROC, (LONG_PTR)&WndProc);

        Sleep(150);

        return 0;
    }

    int Cleanup(GarrysMod::Lua::ILuaBase *LUA)
    {
        FreeConsole();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID &)oldFunc, EndSceneHook);

        LONG lError = DetourTransactionCommit();
        if (lError != NO_ERROR)
        {
            MessageBox(HWND_DESKTOP, L"failed to revert detour", L"puffy", MB_OK);
            return FALSE;
        }
        else
        {
            error(LUA, "Reverted detour on EndScene..");

            ImGui_ImplDX9_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();

            error(LUA, "Restoring original window procedure for gmod..");

            SetWindowLongPtr(GetProcessWindow(), GWLP_WNDPROC, originalWNDPROC);
        }
        return 0;
    }
    // used for restoring the EndScene
}