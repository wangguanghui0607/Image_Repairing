#pragma once
#include <string>
#include "Pixel.h"

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Image
{
private:
	
protected:
	int imageSize;
	int width;
	int height;
	int brightenness;
	string fileName;
	unsigned char *imageData;
	Pixel *pixels;

	void initPram();
	int getPosition(int x, int y);
public:
	Image();
	Image(string fName);
	Image(unsigned char *imageData, int width, int height);

	void loadImage(string fName);
	void initImage();
	void initImage(int red, int green, int blue);

	void saveImage();
	void saveImage(string fName);

	void copyImage();

	string getFileName();
	int getWidth();
	int getHeight();
	unsigned char * getImageData();
	Pixel * getPixels();

	~Image(void);
};

