#include "mainHook.h"
#include <d3d9.h>
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

namespace DXHook
{
	EndScene oldFunc;
	void *d3d9Device[119];
	LPDIRECT3DDEVICE9 device;
	bool gotDevice = false;

	HRESULT __stdcall EndSceneHook(LPDIRECT3DDEVICE9 pDevice)
	{
		if (!gotDevice)
		{
			gotDevice = true;
			device = pDevice;

			ImGui_ImplDX9_Init(device);
		}

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplDX9_NewFrame();

		ImGui::NewFrame();
		DXHook::UpdateImGUI();

		// removing SetNextWindowFocus here helps the window's popups work properly
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