#include "stdafx.h"
#include "Image.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

Image::Image(void)
{
	initPram();
}

Image::Image(string fName)
{
	initPram();
	fileName = fName;
	loadImage(fileName);
}

Image::Image(unsigned char *iData, int w, int h)
{
	imageData = iData;
	width = w;
	height = h;
}

void Image::initPram()
{
	imageSize = 3000;
	width = 0;
	height = 0;
	brightenness = 255;
	imageData = NULL;
}

int Image::getPosition(int x, int y)
{
	return (width != 0) ? (y * width + x) : 0;
}

int Image::getWidth()
{
	return width;
}

int Image::getHeight()
{
	return height;
}

unsigned char * Image::getImageData()
{
	return imageData;
}

Pixel * Image::getPixels()
{
	return pixels;
}

void Image::loadImage(string name)
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

	imageData = new unsigned char[width*height*3];

	// display property of image
	cout<<"width："<<width<<" height："<<height<<"\n";
	cout<<"maxGray: "<<maxGray<<"\n";

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

	ifp.close();
}

void Image::initImage()
{
	initImage(0, 0, 0);
}

void Image::initImage(int red, int green, int blue)
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

void Image::copyImage()
{
	saveImage("");
}

void Image::saveImage()
{
	saveImage(fileName);
}

void Image::saveImage(string fname)
{
	string ofName;
	if (fname[0] == '\0')
	{
		cout<<"出力ファイル名（拡張子はppm）: ";
		cin>>ofName;
	} else {
		ofName = fname;
	}

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
	cout<<"画像は正常に出力されました。"<<endl;
}

string Image::getFileName(){
	return fileName;
}

Image::~Image(void)
{
}
