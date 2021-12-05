#include "stdafx.h"

#include "window.h"

#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#include <windowsx.h>

#include "auto.h"
#include "draw.h"
#include "game.h"
#include "resource.h"

HWND g_hWnd;

HWND g_hBtnDifficulty;
HWND g_hBtnRandom, g_hBtnSolve;
HWND g_hBtnAuto, g_hBtnStop;
HWND g_hBtnImage;

bool g_isFullScreen = false;
RECT g_lastWindowRect;
LONG_PTR g_lastWindowStyle;

INT_PTR CALLBACK AboutDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hWnd, 0);
			return TRUE;
		}
		break;

	case WM_INITDIALOG:
		
	{
		SendDlgItemMessage(hWnd, IDC_STATIC1, STM_SETICON, (WPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)), 0);
		HWND hWndOwner = GetParent(hWnd);
		RECT rcDlg, rcOwner;

		GetWindowRect(hWnd, &rcDlg);
		GetWindowRect(hWndOwner, &rcOwner);

		SetWindowPos(hWnd, HWND_TOP,
			(rcOwner.left + rcOwner.right - (rcDlg.right - rcDlg.left)) / 2,
			(rcOwner.top + rcOwner.bottom - (rcDlg.bottom - rcDlg.top)) / 2,
			0, 0, SWP_NOSIZE);
	}
	return TRUE;

	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:          // Fall through to the next case.

		case NM_RETURN:
			// NOT supported MultiByte
			ShellExecute(NULL, L"open", ((PNMLINK)lParam)->item.szUrl, NULL, NULL, SW_SHOW);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

LRESULT CALLBACK BtnWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		SendMessage(g_hWnd, uMsg, wParam, lParam);
		return 0;

	case WM_KEYUP:
		SendMessage(g_hWnd, uMsg, wParam, lParam);
		return 0;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void OnSelectImage()
{
	TCHAR fileName[260] = _T("");
	OPENFILENAME ofn = {sizeof(OPENFILENAME)};
	ofn.hwndOwner = g_hWnd;                     
	ofn.lpstrFilter = _T("Image files\0*.bmp;*.jpg;*.jpeg;*.gif;*.png;*.ico\0All files\0*.*\0");	              
	ofn.nFilterIndex = 1;                       
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 260;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	if (GetOpenFileName(&ofn))
	{
		SetImageFile(fileName);
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		if (lParam && HIWORD(wParam) == BN_CLICKED)
		{
			switch (LOWORD(wParam))
			{
			case MBTN_DIFFICULTY:
				if (g_threadRunning) break;
				if (g_boardSize == 3) g_boardSize = 4;
				else if (g_boardSize == 4) g_boardSize = 5;
				else g_boardSize = 3;
				CalculateLayout();
				InvalidateRect(hWnd, NULL, FALSE);
				break;

			case MBTN_RANDOM:
				if (g_threadRunning) break;
				if (g_boardSize == 3) g_board3.random_shuffle();
				else if (g_boardSize == 4) g_board4.random_shuffle();
				else g_board5.random_shuffle();
				InvalidateRect(hWnd, NULL, FALSE);
				break;

			case MBTN_SOLVE:
				if (g_threadRunning) break;
				if (g_boardSize == 3) g_board3.clear();
				else if (g_boardSize == 4) g_board4.clear();
				else g_board5.clear();
				InvalidateRect(hWnd, NULL, FALSE);
				break;

			case MBTN_AUTO:
				if (g_threadRunning) break;
				g_threadRunning = true;
				InvalidateRect(g_hWnd, NULL, FALSE);
				CloseHandle(CreateThread(NULL, 0, AutoComplete, NULL, 0, NULL));
				break;

			case MBTN_STOP:
				g_threadRunning = false;
				InvalidateRect(hWnd, NULL, FALSE);
				break;

			case MBTN_IMAGE:
				OnSelectImage();
				break;
			}
		}
		return 0;

	case WM_CREATE:

		if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pIWICFactory))))
		{
			if (FAILED(CoCreateInstance(CLSID_WICImagingFactory1, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&g_pIWICFactory))))
			{
				return -1;
			}
		}
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pID2D1Factory)))
		{
			return -1;
		}
		DPIScale::Initialize();

		{
			HMENU hMenu = GetSystemMenu(hWnd, FALSE);
			AppendMenu(hMenu, MF_SEPARATOR, 0, 0);         
			AppendMenu(hMenu, MF_STRING, MMENU_ABOUT, _T("Puzzle Game..."));
		}
		{
			HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

#ifndef _UNICODE
#error ComCtl32.dll version 6 is Unicode only. \
For more information, please visit https://docs.microsoft.com/en-us/windows/desktop/controls/subclassing-overview
#endif
			g_hBtnDifficulty = CreateWindow(_T("BUTTON"), _T("Change difficulty"), WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
				0, 0, 0, 0, hWnd, (HMENU)MBTN_DIFFICULTY, hInstance, NULL);
			SetWindowSubclass(g_hBtnDifficulty, BtnWindowProc, MBTN_DIFFICULTY, 0);

			g_hBtnRandom = CreateWindow(_T("BUTTON"), _T("Shuffle"), WS_CHILD | BS_PUSHBUTTON,
				0, 0, 0, 0, hWnd, (HMENU)MBTN_RANDOM, hInstance, NULL);
			SetWindowSubclass(g_hBtnRandom, BtnWindowProc, MBTN_RANDOM, 0);

			g_hBtnSolve = CreateWindow(_T("BUTTON"), _T("Reset"), WS_CHILD | BS_PUSHBUTTON,
				0, 0, 0, 0, hWnd, (HMENU)MBTN_SOLVE, hInstance, NULL);
			SetWindowSubclass(g_hBtnSolve, BtnWindowProc, MBTN_SOLVE, 0);

			g_hBtnAuto = CreateWindow(_T("BUTTON"), _T("Autosolve"), WS_CHILD | BS_PUSHBUTTON,
				0, 0, 0, 0, hWnd, (HMENU)MBTN_AUTO, hInstance, NULL);
			SetWindowSubclass(g_hBtnAuto, BtnWindowProc, MBTN_AUTO, 0);

			g_hBtnStop = CreateWindow(_T("BUTTON"), _T("Stop"), WS_CHILD | BS_PUSHBUTTON,
				0, 0, 0, 0, hWnd, (HMENU)MBTN_STOP, hInstance, NULL);
			SetWindowSubclass(g_hBtnStop, BtnWindowProc, MBTN_STOP, 0);

			g_hBtnImage = CreateWindow(_T("BUTTON"), _T("Choose image"), WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
				0, 0, 0, 0, hWnd, (HMENU)MBTN_IMAGE, hInstance, NULL);
			SetWindowSubclass(g_hBtnImage, BtnWindowProc, MBTN_IMAGE, 0);
		}

		return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		SafeRelease(g_pID2D1Factory);
		SafeRelease(g_pIWICFactory);
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F11:
			if (lParam & 0x40000000) break;
			if (!g_isFullScreen)
			{
				GetWindowRect(hWnd, &g_lastWindowRect);
				g_lastWindowStyle = SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
				SetWindowPos(hWnd, 0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED | SWP_NOZORDER);
			}
			else
			{
				SetWindowLongPtr(hWnd, GWL_STYLE, g_lastWindowStyle);
				SetWindowPos(hWnd, 0, g_lastWindowRect.left, g_lastWindowRect.top, 
					g_lastWindowRect.right - g_lastWindowRect.left, g_lastWindowRect.bottom - g_lastWindowRect.top, SWP_FRAMECHANGED | SWP_NOZORDER);
			}
			g_isFullScreen ^= 1;
			break;

		case 'W':

		case VK_UP:
			if (g_threadRunning) break;
			OnMove(MoveInfo::MOVE_UP);
			break;

		case 'S':

		case VK_DOWN:
			if (g_threadRunning) break;
			OnMove(MoveInfo::MOVE_DOWN);
			break;

		case 'A':

		case VK_LEFT:
			if (g_threadRunning) break;
			OnMove(MoveInfo::MOVE_LEFT);
			break;

		case 'D':

		case VK_RIGHT:
			if (g_threadRunning) break;
			OnMove(MoveInfo::MOVE_RIGHT);
			break;

		case VK_SPACE:
			if (!g_isPreview)
			{
				g_isPreview = true;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		}
		return 0;

	case WM_KEYUP:
		switch (wParam)
		{
		case VK_SPACE:
			if (g_isPreview)
			{
				g_isPreview = false;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			break;
		}
		return 0;

	case WM_LBUTTONDOWN:
		if (g_threadRunning) break;
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_LBUTTONUP:
		OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_SIZE:
		Resize();
		return 0;

	case WM_SYSCOMMAND:
		switch (wParam & 0xFFF0)
		{
		case MMENU_ABOUT:
			DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDialogProc);
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
