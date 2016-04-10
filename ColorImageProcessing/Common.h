#include "Pixel.h"
#include "ColorImage.h"

#pragma once


class Common
{
public:
	Common(void);

	static bool colorRange(PixelRGB targ, PixelRGB from, PixelRGB to);
	static bool valueRange(double targ, double min, double max);
	static bool isSameColor(PixelRGB r1, PixelRGB r2);
	static double gaussianBlur(double sigma, int x, int y);
	static ColorImage setObjectFlag(ColorImage ob, int flag, bool isCut);

	~Common(void);
};

