#pragma once

#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <wincodec.h>

#include "game.h"

extern ID2D1Factory            *g_pID2D1Factory;
extern ID2D1HwndRenderTarget   *g_pRenderTarget;
extern ID2D1SolidColorBrush    *g_pBrush;
extern IWICImagingFactory      *g_pIWICFactory;

template <class T> inline void SafeRelease(T *&ppT)
{
	if (ppT)
	{
		ppT->Release();
		ppT = NULL;
	}
}

class DPIScale
{
	static float scale;

public:
	static void Initialize()
	{
		scale = GetDpiForSystem() / 96.0f;
	}

	template <typename T>
	static D2D1_POINT_2F PixelsToDips(T x, T y)
	{
		return D2D1::Point2F(static_cast<float>(x) / scale, static_cast<float>(y) / scale);
	}

	template <typename T>
	static float PixelsToDips(T x)
	{
		return static_cast<float>(x) / scale;
	}
};

void CalculateLayout();

HRESULT CreateGraphicsResources();

HRESULT CreateMyBitmapFromWicBitmap(IWICBitmapSource * pIWICBitmapSource);

void DiscardGraphicsResources();

HRESULT LoadResourceBitmap();

void OnLButtonDown(int pixelX, int pixelY, DWORD flags);

void OnMove(MoveInfo mov);

void OnPaint();

void PaintBoard();

void PaintButton();

void Resize();

HRESULT SetImageFile(PCTSTR fileName);