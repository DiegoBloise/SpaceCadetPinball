#include "psp_ge.h"
#include "gdrv.h"

#include <pspkernel.h>
#include <pspgu.h>
#include <pspdisplay.h>
#include <cstring>

static int screen_width = 480;
static int screen_height = 272;
static bool linear_filter = false;
static bool initialized = false;

static inline unsigned int swapRB(unsigned int c)
{
	return (c & 0xFF00FF00) | ((c & 0x00FF0000) >> 16) | ((c & 0x000000FF) << 16);
}

void psp_ge::init(int width, int height)
{
	screen_width = width;
	screen_height = height;

	void* vram = sceGeEdramGetAddr();

	// Clear entire EDRAM to black before displaying (removes splash screen residue)
	std::memset(vram, 0, 512 * 512 * 4);

	sceGuDisplay(GU_FALSE);
	sceGuInit();

	sceGuStart(GU_DIRECT, nullptr);
	sceGuDrawBuffer(GU_PSM_8888, vram, 512);
	sceGuDispBuffer(screen_width, screen_height, vram, 512);
	sceGuDepthBuffer((void*)((unsigned int)vram + 512 * 512 * 4), 512);
	sceGuFinish();
	sceGuSync(0, 0);

	sceGuDisplay(GU_TRUE);
	sceDisplaySetFrameBuf(vram, 512, GU_PSM_8888, 0);

	initialized = true;
}

void psp_ge::shutdown()
{
	if (initialized)
	{
		sceGuDisplay(GU_FALSE);
		sceGuTerm();
		initialized = false;
	}
}

void psp_ge::beginFrame()
{
	// Clear the active screen area each frame to prevent ghosting
	unsigned char* vram = static_cast<unsigned char*>(sceGeEdramGetAddr());
	for (int y = 0; y < screen_height; y++)
		std::memset(vram + y * 512 * 4, 0, screen_width * 4);
}

void psp_ge::drawQuad(gdrv_bitmap8* bmp, int srcX, int srcY, int srcW, int srcH,
                       int dstX, int dstY, int dstW, int dstH)
{
	if (!bmp || !bmp->BmpBufPtr1)
		return;

	unsigned char* vram = static_cast<unsigned char*>(sceGeEdramGetAddr());

	for (int y = 0; y < dstH; y++)
	{
		int srcRow = srcY + y * srcH / dstH;
		if (srcRow >= bmp->Height)
			continue;

		int dstRow = dstY + y;
		if (dstRow >= screen_height)
			break;
		if (dstRow < 0)
			continue;

		const ColorRgba* srcLine = bmp->BmpBufPtr1 + srcRow * bmp->Width + srcX;
		ColorRgba* dstLine = reinterpret_cast<ColorRgba*>(vram + dstRow * 512 * 4 + dstX * 4);

		for (int x = 0; x < dstW; x++)
		{
			int srcCol = x * srcW / dstW;
			if (srcCol >= bmp->Width)
				break;
			int dstCol = dstX + x;
			if (dstCol >= screen_width)
				break;
			if (dstCol < 0)
				continue;
			dstLine[x] = ColorRgba(swapRB(srcLine[srcCol].Color));
		}
	}
}

void psp_ge::endFrame()
{
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
