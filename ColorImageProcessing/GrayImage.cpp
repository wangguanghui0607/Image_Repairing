#include "stdafx.h"
#include "GrayImage.h"
#include <string>
#include <iostream>
#include <fstream>
#include <hash_map>

#define PI 3.141592653589793238462
#define INFINITY 1.0e308
using namespace std;

GrayImage::GrayImage(void)
{
	initPram();
}

GrayImage::GrayImage(string filename)
{
	initPram();
	fileName = filename;
	loadImage(fileName);
}

GrayImage::GrayImage(unsigned char *data, int w, int h)
{
	imageData = data;
	width = w;
	height = h;
}

void GrayImage::loadImage(string name)
{
	if (name[0] != '\0')
	{
		fileName = name;
	} else
	{
		cout<<"please input a fileName(**.pgm): ";
		cin>>fileName;
	}
	
	string buffer;

	ifstream ifp;

	ifp.open(fileName, ios::in | ios::binary);

	if (!ifp)
	{
		cout<<"input file can't be open";
		exit(1);
	}

	// check format
	while (true)
	{
		getline(ifp, buffer);
		if (buffer[0] != '#')
		{
			if (buffer != "P5")
			{
				cout<<"input file format is incorrect.\n";
				exit(1);
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

	imageData = new unsigned char[width * height];

	// display property of image
	cout<<"width："<<width<<" height："<<height<<"\n";
	cout<<"maxGray: "<<maxGray<<"\n";

	if (width > imageSize || height > imageSize || maxGray > brightenness)
	{
		cout<<"maxGray is incorrect.\n";
		exit(1);
	}

	int size = width * height;
	for (int x = 0; x < size; x++)
	{
		imageData[x] = (unsigned char)ifp.get(); 
	}

	ifp.close();
}

void GrayImage::initImage()
{
	initImage(0);
}

void GrayImage::initImage(int bright)
{
	if (!imageData)
	{
		return;
	}

	int x = 0, size = width * height;

	while (x < size)
	{
		imageData[x] = bright;
		x++;
	}
}

void GrayImage::createHistogram()
{
	int i, maxNumOfBri, minNumOfBri, x = 0, size = width * height;
	int bright[256];

	ofstream ofp;
	ofp.open("images/gray/histoGram.csv", ios::out | ios::binary | ios::trunc);

	for (i = 0; i < 256; i++)
	{
		bright[i] = 0;
	}

	while (x < size)
	{
		bright[imageData[x]]++;
		x++;
	}

	maxNumOfBri = 0;
	minNumOfBri = 100000;
	for (i = 0; i < 256; i++)
	{
		if (bright[i] > maxNumOfBri)
		{
			maxNumOfBri = bright[i];
		}

		if (bright[i] < minNumOfBri)
		{
			minNumOfBri = bright[i];
		}
	}

	for (i = 0; i < 256; i++)
	{
		bright[i] = bright[i] * 256 / (maxNumOfBri - minNumOfBri);
	}

	for (i = 0; i < 256; i++)
	{
		ofp<<bright[i]<<",";
	}

	ofp.close();
}

void GrayImage::setImageData(unsigned char *iData)
{
	imageData = iData;
}

void GrayImage::setWidthHeight(int w, int h)
{
	width = w;
	height = h;
}

// size must be a odd number.
GrayImage GrayImage::getPatch(int x, int y, int s){
	unsigned char *patchData = NULL;
	int i, j, patch_width, start, offset;

	patch_width = s / 2;
	patchData = new unsigned char[s * s];

	offset = 0;
	GrayImage patch;
	memset(patchData, 255, patch_width * patch_width * sizeof(unsigned char));
	patch.setImageData(patchData);
	patch.setWidthHeight(s, s);

	if (patch_width > 0 && x - patch_width >= 0 && y - patch_width >= 0 && x + patch_width < width && y + patch_width < height )
	{
		
		start = (y - patch_width) * width + x - patch_width;
		for (i = 0; i < s; i++)
		{
			for (j = 0; j < s; j++)
			{
				offset = i * s + j;
				patchData[offset] = imageData[start + i * width + j];
			}
		}
		patch.setImageData(patchData);
	} else
	{
		cout<<"error 1."<<endl;
		cout<<"x: "<<x<<", y: "<<y<<", width: "<<patch_width<<endl;
	}
	return patch;
}

GrayImage GrayImage::getPatch(int startX, int startY, int patchW, int patchH)
{
	unsigned char *patchData = NULL;
	int i, j, start, offset;
	offset = 0;
	GrayImage patch;


	patchData = new unsigned char[patchW * patchH];
	memset(patchData, 255, patchW * patchH * sizeof(unsigned char));

	patch.setImageData(patchData);
	patch.setWidthHeight(patchW, patchH);

	if (startX >= 0 && startX + patchW < width && startY >= 0 && startY + patchH < height)
	{
		
		start = startY * width + startX;

		for (i = 0; i < patchH; i++)
		{
			for (j = 0; j < patchW; j++)
			{
				offset = i * patchW + j;
				patchData[offset] = imageData[start + i * width + j];
			}
		}

		patch.setImageData(patchData);
		patch.setWidthHeight(patchW, patchH);
	} else
	{
		cout<<"error 2."<<endl;
	}

	return patch;
}

void GrayImage::pastePatch(GrayImage patch, int startX, int startY)
{
	unsigned char* patchData;
	int patchW, patchH, i, j, start, offset;

	patchW = patch.getWidth();
	patchH = patch.getHeight();
	patchData = patch.getImageData();

	if (patchW > 0 && patchH > 0 && patchW <= width && patchH <= height && patchData != NULL)
	{
		start = startY * width + startX;
		for (i = 0; i < patchH && startY + i < height; i++)
		{
			for (j = 0; j < patchW && startX + j < width; j++)
			{
				offset = i * patchW + j;
				imageData[start + i * width + j] = patchData[offset];
			}
		}
	}
}

double GrayImage::calcSSD(GrayImage patch1, GrayImage patch2, int nonTargPix)
{
	int i, patchW, patchH;
	unsigned char *patch1_data, *patch2_data;
	double size, ssd;
	ssd = 0.0;

	if (patch1.getWidth() != patch2.getWidth() || patch1.getHeight() != patch2.getHeight())
	{
		return ssd;
	}

	patch1_data = patch1.getImageData();
	patch2_data = patch2.getImageData();
	patchW = patch1.getWidth();
	patchH = patch1.getHeight();

	size = patchW * patchH;

	for (i = 0; i < size; i++)
	{
		if (patch1_data[i] == nonTargPix)
		{
			continue;
		}
		ssd += (patch1_data[i] - patch2_data[i]) * (patch1_data[i] - patch2_data[i]);
	}

	return ssd / size;
}

/*
 calculate two-dimensional Gaussian kernel
*/
double GrayImage::calcGaussianKernel(GrayImage patch1, GrayImage patch2, int ssd, int nonTargPix)
{
	int i, size, patchW, patchH;
	double varience, guasKernal; 
	unsigned char *patch1_data, *patch2_data;

	guasKernal = 0;

	if (patch1.getWidth() != patch2.getWidth() || patch1.getHeight() != patch2.getHeight())
	{
		return guasKernal;
	}

	patch1_data = patch1.getImageData();
	patch2_data = patch2.getImageData();
	patchW = patch1.getWidth();
	patchH = patch1.getHeight();

	varience = (double)ssd / (patchW * patchH);

	if (varience == 0)
	{
		return guasKernal;
	}

	size = patchW * patchH;
	for (i = 0; i < size; i++)
	{
		if (patch1_data[i] == nonTargPix)
		{
			continue;
		}
		guasKernal = exp(-(patch1_data[i] * patch1_data[i]  + patch2_data[i] * patch2_data[i]) / (2 * varience));
		guasKernal = guasKernal / (2 * PI * varience);
	}

	return guasKernal;
}

/*
	サンプル画像から参照パッチ画像と類似度が高いパッチを取る
*/
GrayImage GrayImage::getBestPatch(GrayImage patch, GrayImage sample, int nonTargPix)
{
	int i, j, patchW, patchH, sampleW, sampleH, difW, difH, ssd;
	double d_perc, min_d_perc;
	GrayImage tmpPatch, bestPatch;
	patchW = patch.getWidth();
	patchH = patch.getHeight();
	sampleW = sample.getWidth();
	sampleH = sample.getHeight();

	difW = sampleW - patchW;
	difH = sampleH - patchH;
	min_d_perc = INFINITY;
	
	for (i = 0; i < difH; i++)
	{
		for (j = 0; j < difW; j++)
		{
			tmpPatch = sample.getPatch(j, i, patchW, patchH);
			d_perc = calcSSD(patch, tmpPatch, nonTargPix);
			
			if (d_perc <= 40)
			{
				min_d_perc = d_perc;
				return tmpPatch;
			}
			if (d_perc == 0 || d_perc < min_d_perc)
			{
				min_d_perc = d_perc;
				bestPatch = tmpPatch;
			}
		}
	}
//	cout<<"min_d_perc: "<<min_d_perc<<endl;
	return bestPatch;
}

GrayImage *GrayImage::getBestPatches(GrayImage bestPatch, GrayImage sample, double threshold, int numOfRes, int nonTargPix)
{
	int i, j, patchW, patchH, sampleW, sampleH, difW, difH, patchCount;
	double d_perc, ssd;
	GrayImage tmpPatch, *bestPatches;
	patchW = bestPatch.getWidth();
	patchH = bestPatch.getHeight();
	sampleW = sample.getWidth();
	sampleH = sample.getHeight();

	difW = sampleW - patchW;
	difH = sampleH - patchH;

	hash_map<double, GrayImage> perc_patches;

	bestPatches = new GrayImage[difW * difH];
	patchCount = 0;
	d_perc = 0.0;
	for (i = 0; i < difH; i++)
	{
		for (j = 0; j < difW; j++)
		{
			tmpPatch = sample.getPatch(j, i, patchW, patchH);
			ssd = calcSSD(bestPatch, tmpPatch, nonTargPix);
			d_perc = ssd * calcGaussianKernel(bestPatch, tmpPatch, ssd, nonTargPix);
			
			if (d_perc * 10000000000 < threshold)
			{
				perc_patches.insert(std::pair<double, GrayImage>(d_perc, tmpPatch));
				bestPatches[patchCount] = tmpPatch;
				patchCount++;
			}
		}
	}

	hash_map<double, GrayImage>::iterator begin, end, itr;
	begin = perc_patches.begin();
	end = perc_patches.end();

	for(itr = begin, i = 0; itr != end && i < numOfRes; itr++, i++){
	//	cout<<itr->first<<":  "<<endl;
		bestPatches[i] = itr->second;
	//	cout<<"width: "<<itr->second.getWidth()<<endl;
	}

	return bestPatches;
}

void GrayImage::copyImage()
{
	saveImage("");
}

void GrayImage::saveImage()
{
	saveImage(fileName);
}

void GrayImage::saveImage(string fname)
{
	string ofName;
	if (fname[0] == '\0')
	{
		cout<<"input file's name(XXX.pgm): ";
		cin>>ofName;
	} else {
		ofName = fname;
	}
	
	if (imageData[0] == '\0')
	{
		cout<<"image data is null"<<endl;
		return;
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

	int size = width * height;
	for (int x = 0; x < size; x++)
	{
		ofp<<imageData[x];
	}

	ofp.close();
	cout<<"Gray image has been saved successfully."<<endl;
}

GrayImage::~GrayImage(void)
{
}
