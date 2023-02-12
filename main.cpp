#define NOMINMAX
#include <windows.h>
#include <filesystem>

#include "ui.h"
#include "win_basic.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_ui ui;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		ui.paint(hWnd);
		EndPaint(hWnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	exe_path = get_exe_path(hInstance);
	return run_windows_app(L"journal", hInstance, WndProc, nCmdShow);
}
