#include "stdafx.h"
#include "Common.h"
#include<math.h>
#include<iostream>

#define PI 3.14159265258979

using namespace std;

Common::Common(void)
{
}

bool Common::colorRange(PixelRGB targ, PixelRGB from, PixelRGB to)
{
	bool r, g, b;
	r = g = b = false;

	r = (from.r > to.r) ? (targ.r >= to.r && targ.r <= from.r) : (targ.r <= to.r && targ.r >= from.r);
	g = (from.g > to.g) ? (targ.g >= to.g && targ.g <= from.g) : (targ.g <= to.g && targ.g >= from.g);
	b = (from.b > to.b) ? (targ.b >= to.b && targ.b <= from.b) : (targ.b <= to.b && targ.b >= from.b);

	return r && g && b;
}

bool Common::valueRange(double targ, double min, double max)
{
	return targ >= min && targ < max;
}

bool Common::isSameColor(PixelRGB r1, PixelRGB r2)
{
	return (r1.r == r2.r) && (r1.g == r2.g) && (r1.b == r2.b);
}

double Common::gaussianBlur(double sigma, int x, int y){
	if (sigma != 0 && x >= 0 && y >= 0)
	{
		return exp(-(x*x + y*y) / (2*sigma*sigma)) / (2 * PI * sigma * sigma);
	} else
	{
		return 1;
	}
	
}

ColorImage Common::setObjectFlag(ColorImage object, int flag, bool isCut)
{
//	object.imageDataToPixels();

	int i, j, w, h, count;

	double rate = 0.0;
	Pixel *pixels, tmpPixel;
	w = object.getWidth();
	h = object.getHeight();
	count = 0;

	int obRight, obLeft, obTop, obBottom;
	obRight = obBottom = 0;
	obLeft = w;
	obTop = h;
	bool setTop = false;
	
	pixels = object.getPixels();

	for (i = 0; i < h-1; i++)
	{
		for (j = 0; j < w-1; j++)
		{
			tmpPixel = pixels[i * w + j];

			if (tmpPixel.getFlag() == 2 || tmpPixel.getFlag() == 3 ||
				(tmpPixel.getRGB().r == 0 && tmpPixel.getRGB().g == 0 && tmpPixel.getRGB().b == 0))
			{
			//	cout<<"missing"<<endl;
				continue;
			}

			if (tmpPixel.getRGB().r < 230 || tmpPixel.getRGB().g < 230 || tmpPixel.getRGB().b < 230 )
			{
				pixels[i * w + j].setFlag(flag);
				count++;

				if (!setTop)
				{
					obTop = i;
					setTop = true;
				}

				obBottom = i;

				if (j > obRight)
				{
					obRight = j;
				}

				if (j < obLeft)
				{
					obLeft = j;
				}
			} else
			{
				pixels[i * w + j].setFlag(4);
			}
		}
	}
	object.setPixels(pixels);


	if (!isCut)
	{
		return object;
	}

//	cout<<obLeft<<","<<obTop<<"; "<<obRight<<","<<obBottom<<endl;

	// create a new image for object
	// save object
	ColorImage obImg = ColorImage();
	int newW, newH;
	newW = obRight - obLeft + 1;
	newH = obBottom - obTop + 1;
	obImg.setWidthHeight(newW, newH);

	Pixel *newPixels;
	newPixels = new Pixel[newW * newH];

	for (i = 0; i < newH; i++)
	{
		for (j = 0; j < newW; j++)
		{
			newPixels[i * newW + j] = pixels[(obTop + i) * w + j + obLeft];
		}
	}

	obImg.setPixels(newPixels);

	obImg.saveImage("images/color/refOb/targOb002000.ppm");
	return obImg;
}


Common::~Common(void)
{
}
