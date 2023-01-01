#include "mainHook.h"
#include <d3d9.h>
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include <Windows.h>
#include <iostream>

namespace DXHook
{
	EndScene oldFunc;
	void* d3d9Device[119];
	LPDIRECT3DDEVICE9 device;
	bool gotDevice = false;

	HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 pDevice)
	{
		if (!gotDevice) {
			gotDevice = true;
			device = pDevice;

			ImGui_ImplDX9_Init(device);
		}

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX9_NewFrame();

		ImGui::NewFrame();
		DXHook::UpdateImGUI();

		ImGui::SetNextWindowFocus();

		static bool showWnd = true;
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");

		ImGui::Text("This is some useful text.");

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::ColorEdit3("clear color", (float*)&showWnd);

		if (ImGui::Button("Button"))
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		ImGui::ShowDemoWindow();

		ImGui::EndFrame();

		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

        HRESULT result = oldFunc(pDevice);

        return result;
	}
}