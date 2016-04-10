#include "stdafx.h"
#include "ColorImage.h"
#include <string>
#include <iostream>
#include <fstream>


using namespace std;

ColorImage::ColorImage(void)
{
//	initPram();
	imageData_R = NULL;
	imageData_G = NULL;
	imageData_B = NULL;
	imageData = NULL;
}

ColorImage::ColorImage(string fName)
{
//	initPram();
	imageData_R = NULL;
	imageData_G = NULL;
	imageData_B = NULL;
	imageData = NULL;

	fileName = fName;
	loadImage(fileName);
}

void ColorImage::setPixels(Pixel *p)
{
	pixels = p;
}

void ColorImage::setImageData(unsigned char *imageDataR, unsigned char *imageDataG, unsigned char *imageDataB)
{
	imageData_R = imageDataR;
	imageData_G = imageDataG;
	imageData_B = imageDataB;
}

void ColorImage::rgbTransfToGray()
{
	int i;
	double gray = 0.0;
	Pixel tmpPixel;
	PixelRGB tmpRGB;

	for (i = 0; i < width * height; i++)
	{
		tmpPixel = pixels[i];
		tmpRGB = tmpPixel.getRGB();
		gray = 0.298912 * tmpRGB.r + 0.586611 * tmpRGB.g + 0.114478 * tmpRGB.b;
		tmpPixel.setGrayValue(gray);
	}
}

void ColorImage::setWidthHeight(int w, int h)
{
	width = w;
	height = h;

	//pixels = new Pixel[width * height];
	//imageData_R = new unsigned char[width * height];
	//imageData_G = new unsigned char[width * height];
	//imageData_B = new unsigned char[width * height];
	//imageData = new unsigned char[width * height * 3];
}

Pixel * ColorImage::getPixels()
{
	return pixels;
}

void ColorImage::splitRGB(){
	if (!imageData)
	{
		if (!pixels)
		{
			return;
		} else
		{
			PixelsToimageData();
		}
		
	}

	int x = 0, size = width * height;

	if (imageData_R == NULL)
	{
		imageData_R = new unsigned char[size];
	}

	if (imageData_G == NULL)
	{
		imageData_G = new unsigned char[size];
	}

	if (imageData_B == NULL)
	{
		imageData_B = new unsigned char[size];
	}
	//imageData_R = new unsigned char[size];
	//imageData_G = new unsigned char[size];
	//imageData_B = new unsigned char[size];

	//pixels = new Pixel[size];
	if (pixels == NULL)
	{
	//	pixels = new Pixel[size];
		imageDataToPixels();
	}
	Pixel tmpPixel;
	PixelRGB rgb;
	PixelPos pos;

	while (x < size)
	{
		rgb.r = imageData_R[x] = imageData[x * 3];
		rgb.g = imageData_G[x] = imageData[x * 3 + 1]; 
		rgb.b = imageData_B[x] = imageData[x * 3 + 2];

		pos.x = x % width;
		pos.y = x / width;

		/*tmpPixel = Pixel(rgb, pos);
		pixels[x] = tmpPixel;*/
		pixels[x].setRGB(rgb);
		x++;
	}
}

void ColorImage::compbineRGB(){
	//if (!imageData)
	//{
	//	return;
	//}
	if (!imageData_R || !imageData_G || !imageData_B)
	{
		return;
	}

	if (imageData == NULL)
	{
		imageData = new unsigned char[width * height * 3];
	}

	int x = 0, size = width * height;

	while (x < size)
	{
		imageData[x * 3] = imageData_R[x];
		imageData[x * 3 + 1]  = imageData_G[x]; 
		imageData[x * 3 + 2] = imageData_B[x];
		x++;
	}
}

void ColorImage::resetRGB(int type, PixelRGB defRgb)
{
	int i, size;
	PixelRGB tmpRgb;
	size = width * height;

	switch (type)
	{
	case 0:
		for (i = 0; i < size; i++)
		{
			tmpRgb = pixels[i].getRGB();
			if (tmpRgb.r > 120 && tmpRgb.r > tmpRgb.g + 30 && tmpRgb.r > tmpRgb.b + 30)
			{
				pixels[i].setRGB(defRgb);
			}
		}
		break;
	case 1:
		for (i = 0; i < size; i++)
		{
			tmpRgb = pixels[i].getRGB();
			if (tmpRgb.g > tmpRgb.r && tmpRgb.g > tmpRgb.b)
			{
				pixels[i].setRGB(defRgb);
			}
		}
		break;
	case 2:
		for (i = 0; i < size; i++)
		{
			tmpRgb = pixels[i].getRGB();
			if (tmpRgb.b > tmpRgb.g && tmpRgb.b > tmpRgb.r)
			{
				pixels[i].setRGB(defRgb);
			}
		}
		break;
	default:
		break;
	}

}

void ColorImage::initImageRGB(int type, int bright){
	int x, size = width * height;

	switch (type)
	{
	case 0:
		for (x = 0; x < size; x++)
		{
			imageData_R[x] = bright;
		}
		break;
	case 1:
		for (x = 0; x < size; x++)
		{
			imageData_G[x] = bright;
		}
		break;
	case 2:
		for (x = 0; x < size; x++)
		{
			imageData_B[x] = bright;
		}
		break;
	default:
		break;
	}
}

unsigned char * ColorImage::getImageDataR()
{
	if (imageData_R == NULL)
	{
		splitRGB();
	}
//	splitRGB();
	return imageData_R;
}

void ColorImage::setOpimalPos(int x, int y)
{
	opimalPosX = x;
	opimalPosY = y;
}

unsigned char * ColorImage::getImageDataG()
{
	if (!imageData_G)
	{
		splitRGB();
	}
//	splitRGB();
	return imageData_G;
}

unsigned char * ColorImage::getImageDataB()
{
	if (!imageData_B)
	{
		splitRGB();
	}
//	splitRGB();
	return imageData_B;
}

ColorImage ColorImage::getPatch(int centerX, int centerY, int size)
{
	GrayImage imageR, imageG, imageB, patchR, patchG, patchB;
	ColorImage resPatch;
	int i, j, halSize;
	Pixel *resPixels;
	resPixels = new Pixel[size * size];
	halSize = size;

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			resPixels[i * size + j] = pixels[(i - halSize + centerY) * width + centerX + j - halSize];
		}
	}

	resPatch = ColorImage();
	resPatch.setWidthHeight(size, size);
	resPatch.setPixels(resPixels);
	resPatch.PixelsToimageData();

	return resPatch;
}

ColorImage ColorImage::getPatch(int startX, int startY, int w, int h)
{
	GrayImage imageR, imageG, imageB, patchR, patchG, patchB;
	ColorImage resPatch;

	splitRGB();
	imageR = GrayImage(imageData_R, width, height);
	imageG = GrayImage(imageData_G, width, height);
	imageB = GrayImage(imageData_B, width, height);

	patchR = imageR.getPatch(startX, startY, w, h);
	patchG = imageG.getPatch(startX, startY, w, h);
	patchB = imageB.getPatch(startX, startY, w, h);

	resPatch = ColorImage();
	resPatch.setImageData(patchR.getImageData(), patchG.getImageData(), patchB.getImageData());
	resPatch.setWidthHeight(w, h);
	resPatch.compbineRGB();

	return resPatch;
}

void ColorImage::loadImage(string name)
{
	if (name[0] != '\0')
	{
		fileName = name;
	} else
	{
		cout<<"please input a fileName(**.ppm): ";
		cin>>fileName;
	}
	
	string buffer;

	ifstream ifp;

	ifp.open(fileName, ios::in | ios::binary);

	if (!ifp)
	{
		cout<<"input file can't be open";
		return;
	}

	// check format
	while (true)
	{
		getline(ifp, buffer);
		if (buffer[0] != '#')
		{
			if (buffer != "P6")
			{
				cout<<"input file format is incorrect.\n";
				return;
			} 
			break;
		}
	}

	// get width and height
	while (true)
	{
		getline(ifp, buffer);
		if (buffer[0] != '#')
		{
			sscanf(buffer.c_str(),"%d %d",&width, &height);
			break;
		}
	}

//	imageData = new unsigned char[width * height * 3];
	imageData_R = new unsigned char[width * height];
	imageData_G = new unsigned char[width * height];
	imageData_B = new unsigned char[width * height];

	// get maxGray
	int maxGray = 0;
	while (true)
	{
		getline(ifp, buffer);
		if (buffer[0] != '#')
		{
			sscanf(buffer.c_str(),"%d",&maxGray);
			break;
		}
	}

	if (imageData == NULL)
	{
		imageData = new unsigned char[width*height*3];
	}
	
	// display property of image
	//cout<<"width: "<<width<<" height: "<<height<<"\n";
	//cout<<"maxGray: "<<maxGray<<"\n";

	if (width > imageSize || height > imageSize || maxGray != brightenness)
	{
		cout<<"maxGray is incorrect.\n";
		return;
	}

	int size = width*height*3;
	for (int x = 0; x < size; x++)
	{
		imageData[x] = (unsigned char)ifp.get(); 
	}

	imageDataToPixels();

	ifp.close();
}

void ColorImage::initImage()
{
	initImage(0, 0, 0);
}

void ColorImage::initImage(int red, int green, int blue)
{
	if (!imageData)
	{
		return;
	}

	int x = 0, size = width * height * 3;

	while (x <size)
	{
		imageData[x] = red;
		imageData[x+1] = green; 
		imageData[x+2] = blue;
		x+=3;
	}
}

void ColorImage::copyImage()
{
	saveImage("");
}

void ColorImage::saveImage()
{
	saveImage(fileName);
}

void ColorImage::imageDataToPixels()
{
	if (width == NULL || height == NULL)
	{
		cout<<"width or height is null."<<endl;
		return;
	}

	if (imageData == NULL && (imageData_R == NULL || imageData_G == NULL || imageData_B == NULL))
	{
		cout<<"image data is null."<<endl;
		return;
	}

	int i, j, size;
	PixelRGB rgb;
	size = width * height;
	
	pixels = new Pixel[size];

	if (imageData)
	{
		for (i = 0, j = 0; i < size; i++, j+=3)
		{
			rgb.r = imageData[j];
			rgb.g = imageData[j+1];
			rgb.b = imageData[j+2];
			pixels[i].setRGB(rgb);
		}
	} else
	{
		for (i = 0; i < size; i++)
		{
			rgb.r = imageData_R[i];
			rgb.g = imageData_G[i];
			rgb.b = imageData_B[i];
			pixels[i].setRGB(rgb);
		}
	}
	
}

void ColorImage::PixelsToimageData()
{
	if (pixels == NULL)
	{
		cout<<"pixels is null."<<endl;
		return;
	}

	int i, j, size;
	PixelRGB rgb;
	size = width * height;

	if (imageData_R == NULL)
	{
		imageData_R = new unsigned char[size * 3];
	}

	if (imageData_G == NULL)
	{
		imageData_G = new unsigned char[size * 3];
	}

	if (imageData_B == NULL)
	{
		imageData_B = new unsigned char[size * 3];
	}

	if (imageData == NULL)
	{
		imageData = new unsigned char[size * 3];
	}

	//imageData_G = new unsigned char[size];
	//imageData_B = new unsigned char[size];

	for (i = 0, j = 0; i < size; i++, j+=3)
	{
		rgb = pixels[i].getRGB();
		imageData_R[i] = imageData[j] = rgb.r;
		imageData_G[i] = imageData[j+1] = rgb.g;
		imageData_B[i] = imageData[j+2] = rgb.b;
	}
}

void ColorImage::saveImage(string fname)
{
	string ofName;
	if (fname[0] == '\0')
	{
		cout<<"input file name.(XX.ppm): ";
		cin>>ofName;
	} else {
		ofName = fname;
	}

	PixelsToimageData();

	ofstream ofp;
	ofp.open(ofName, ios::out | ios::binary | ios::trunc);

	if (!ofp)
	{
		cout<<"output file is incorrect.\n";
		return;
	}

	ofp<<"P6\n";
	ofp<<"# Created by image Processing\n";
	ofp<<width<<" "<<height<<"\n";
	ofp<<brightenness<<"\n";

	int size = width * height*3;
	for (int x = 0; x < size; x++)
	{
		ofp<<imageData[x];
	}

	ofp.close();
//	cout<<"Color image has been saved successfully."<<endl;
}

void ColorImage::saveImageRGB(int type){
	string ofName;
	int x, size = width * height;
	unsigned char *tmpImageData = NULL;
	
	switch (type)
	{
	case 0:
		ofName = fileName + "_R.pgm";
		tmpImageData = imageData_R;
		break;
	case 1:
		ofName = fileName + "_G.pgm";
		tmpImageData = imageData_G;
		break;
	case 2:
		ofName = fileName + "_B.pgm";
		tmpImageData = imageData_B;
		break;
	default:
		break;
	}

	ofstream ofp;
	ofp.open(ofName, ios::out | ios::binary | ios::trunc);

	if (!ofp)
	{
		cout<<"output file is incorrect.\n";
		return;
	}

	ofp<<"P5\n";
	ofp<<"# Created by image Processing\n";
	ofp<<width<<" "<<height<<"\n";
	ofp<<brightenness<<"\n";

	for (x = 0; x < size; x++)
	{
		ofp<<tmpImageData[x];
	}

	ofp.close();
//	cout<<"‰æ‘œ‚Í³í‚Éo—Í‚³‚ê‚Ü‚µ‚½B"<<endl;
}

void ColorImage::rgbToLab()
{
	int i, size;
	Pixel tmpPixel;

	size = width * height;

	for (i = 0; i < size; i++)
	{
		pixels[i].rgbTOLab(pixels[i].getRGB());
	}
}

string ColorImage::getFileName(){
	return fileName;
}

void ColorImage::setCorrespondences(map<int, int> c)
{
	correspondences = c;
}

map<int, int> ColorImage::getCorrespondences()
{
	return correspondences;
}

ColorImage::~ColorImage(void)
{
	//delete[] imageData;
	//delete[] imageData_R;
	//delete[] imageData_G;
	//delete[] imageData_B;
}