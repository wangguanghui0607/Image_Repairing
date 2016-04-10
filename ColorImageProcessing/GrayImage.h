#pragma once
#include "image.h"

class GrayImage: public Image
{
private:
	//int imageSize;
	//int width;
	//int height;
	//int brightenness;
	//string fileName;
	//unsigned char *imageData;
public:
	GrayImage(void);

	GrayImage(string fName);
	GrayImage (unsigned char *imageData, int width, int height);

	void setImageData(unsigned char *imageData);
	void setWidthHeight(int w, int h);

	void loadImage(string fName);
	void initImage();
	void initImage(int bright);
	void createHistogram();

	GrayImage getPatch(int centerX, int centerY, int width);
	GrayImage getPatch(int startX, int startY, int width, int height);

	void pastePatch(GrayImage patch, int startX, int startY);

	void saveImage();
	void saveImage(string fName);

	void copyImage();

	double calcSSD(GrayImage patch1, GrayImage patch2, int nonTargPix);
	double calcGaussianKernel(GrayImage patch1, GrayImage patch2, int ssd, int nonTargPix);
	GrayImage getBestPatch(GrayImage patch, GrayImage sample, int nonTargPix);
	GrayImage *getBestPatches(GrayImage bestPatch, GrayImage sample, double threshold, int numOfRes, int nonTargPix);
//	string getFileName();

	~GrayImage(void);
};

