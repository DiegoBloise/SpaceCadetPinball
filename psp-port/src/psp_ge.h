#pragma once

struct gdrv_bitmap8;

class psp_ge
{
public:
	static void init(int screenWidth, int screenHeight);
	static void shutdown();
	static void beginFrame();
	static void drawQuad(gdrv_bitmap8* bmp, int srcX, int srcY, int srcW, int srcH,
	                     int dstX, int dstY, int dstW, int dstH);
	static void endFrame();
	static void setLinearFilter(bool linear);
	static int getScreenWidth();
	static int getScreenHeight();
};
