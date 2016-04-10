#pragma once
#include "image.h"
#include "GrayImage.h"

using namespace std;

class ColorImage :
	public Image
{
private:
	unsigned char *imageData_R;
	unsigned char *imageData_G;
	unsigned char *imageData_B;
	unsigned char *imageGrayData;
	Pixel *pixels;

	int opimalPosX;
	int opimalPosY;

	// ---- for colorization ----
	map<int, int> correspondences;

	int sizeSeg;
	// --------------------------

public:
	ColorImage(void);

	ColorImage(string fName);
	ColorImage(unsigned char *imageDataR, unsigned char *imageDataG, unsigned char *imageDataB, int width, int height);

	void loadImage(string fName);
	void initImage();
	void initImage(int red, int green, int blue);
	void resetRGB(int type, PixelRGB defRgb);

	void setOpimalPos(int x, int y);

	void setPixels(Pixel *pixels);
	Pixel *getPixels();

	void rgbTransfToGray();

	void setImageData(unsigned char *imageDataR, unsigned char *imageDataG, unsigned char *imageDataB);
	unsigned char * getImageDataR();
	unsigned char * getImageDataG();
	unsigned char * getImageDataB();

	void imageDataToPixels();
	void PixelsToimageData();

	void setWidthHeight(int width, int height);

	void splitRGB();
	void compbineRGB();
	void initImageRGB(int type, int bright);


	void rgbToLab();

	ColorImage getPatch(int centerX, int centerY, int width);
	ColorImage getPatch(int startX, int startY, int width, int height);

	void saveImage();
	void saveImage(string fName);
	void saveImageRGB(int type);

	void copyImage();

	string getFileName();

	// ---- for colorization ----
	void setCorrespondences(map<int, int> c);
	map<int, int> getCorrespondences();

	~ColorImage(void);
};

