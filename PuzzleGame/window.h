#pragma once

#define MBTN_RANDOM         1001
#define MBTN_SOLVE          1002
#define MBTN_AUTO           1003
#define MBTN_STOP           1004
#define MBTN_IMAGE          1005


extern HWND g_hWnd;
extern HWND g_hBtnRandom, g_hBtnSolve;
extern HWND g_hBtnAuto, g_hBtnStop;
extern HWND g_hBtnImage;

extern bool g_isFullScreen;
extern RECT g_lastWindowRect;
extern LONG_PTR g_lastWindowStyle;

INT_PTR CALLBACK AboutDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK BtnWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

void OnSelectImage();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
