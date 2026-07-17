#include "psp_ge.h"
#include "gdrv.h"

#include <pspkernel.h>
#include <pspge.h>
#include <pspdisplay.h>
#include <cstring>

static int screen_width = 480;
static int screen_height = 272;
static bool linear_filter = false;
static bool initialized = false;

static void* frontBuf = nullptr;
static void* backBuf = nullptr;

static inline unsigned int swapRB(unsigned int c)
{
	return (c & 0xFF00FF00) | ((c & 0x00FF0000) >> 16) | ((c & 0x000000FF) << 16);
}

void psp_ge::init(int width, int height)
{
	screen_width = width;
	screen_height = height;

	void* vram = sceGeEdramGetAddr();
	frontBuf = vram;
	backBuf = (void*)((unsigned int)vram + 512 * 512 * 4);

	std::memset(vram, 0, 512 * 512 * 4 * 2);

	sceDisplaySetFrameBuf(frontBuf, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 0);

	initialized = true;
}

void psp_ge::shutdown()
{
	if (initialized)
	{
		initialized = false;
	}
}

void psp_ge::beginFrame()
{
}

void psp_ge::drawQuad(gdrv_bitmap8* bmp, int srcX, int srcY, int srcW, int srcH,
                       int dstX, int dstY, int dstW, int dstH)
{
	if (!bmp || !bmp->BmpBufPtr1 || dstW <= 0 || dstH <= 0)
		return;

	const int bmpW = bmp->Width;
	const int bmpH = bmp->Height;

	// Clip destination to screen bounds
	int clipLeft = 0, clipTop = 0;
	if (dstX < 0) clipLeft = -dstX;
	if (dstY < 0) clipTop = -dstY;
	int clipRight = dstW, clipBottom = dstH;
	if (dstX + dstW > screen_width) clipRight = screen_width - dstX;
	if (dstY + dstH > screen_height) clipBottom = screen_height - dstY;

	if (clipLeft >= clipRight || clipTop >= clipBottom)
		return;

	// Precompute fixed-point step values (16.16 fixed point)
	int srcRowStep = (srcH << 16) / dstH;
	int srcColStep = (srcW << 16) / dstW;

	// Precompute starting source position for clipped region
	int srcRowAcc = clipTop * srcRowStep;

	unsigned char* buf = static_cast<unsigned char*>(backBuf);

	for (int y = clipTop; y < clipBottom; y++)
	{
		int srcRow = srcY + (srcRowAcc >> 16);
		if (srcRow >= bmpH)
			break;

		const ColorRgba* srcLine = bmp->BmpBufPtr1 + srcRow * bmpW + srcX;
		ColorRgba* dstLine = reinterpret_cast<ColorRgba*>(buf + (dstY + y) * 512 * 4 + dstX * 4);

		int srcColAcc = clipLeft * srcColStep;
		for (int x = clipLeft; x < clipRight; x++)
		{
			int srcCol = srcColAcc >> 16;
			if (srcCol >= bmpW)
				break;
			dstLine[x] = ColorRgba(swapRB(srcLine[srcCol].Color));
			srcColAcc += srcColStep;
		}

		srcRowAcc += srcRowStep;
	}
}

void psp_ge::endFrame()
{
	sceDisplayWaitVblankStart();
	sceDisplaySetFrameBuf(backBuf, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);

	void* temp = frontBuf;
	frontBuf = backBuf;
	backBuf = temp;
}

void psp_ge::setLinearFilter(bool linear)
{
	linear_filter = linear;
}

int psp_ge::getScreenWidth()
{
	return screen_width;
}

int psp_ge::getScreenHeight()
{
	return screen_height;
}
