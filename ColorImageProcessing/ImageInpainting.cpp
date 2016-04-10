#include "stdafx.h"
#include "ImageInpainting.h"
#include "Common.h"
#include <time.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <typeinfo.h>

#define INFINITY 1.0e308
#define PI 3.141593
#define HALF_PI 1.570796
#define PATCH_SIZE 9

// Prewitt operator
const int prewittX[3][3] = {{-1, 0, 1},{-1, 0, 1},{-1, 0, 1}};
const int prewittY[3][3] = {{-1, -1, -1},{0, 0, 0},{1, 1, 1}};

// bi-dimensional Gaussian kernel
const double gauKern[5][5] = {{1, 4, 7, 4, 1},{4, 16, 26, 16, 4}, {7, 26, 41, 26, 7},
						{4, 16, 26, 16, 4},{1, 4, 7, 4, 1}};

// 3*3 sobel operator
const int sobelX[3][3] = {{-1, 0, 1},{-2, 0, 2},{-1, 0, 1}};
const int sobelY[3][3] = {{-1, -2, -1},{0, 0, 0},{1, 2, 1}};

// 5*5 sobel operator
const double sobelX5[5][5] = {{2, 1, 0, -1, -2},{3, 2, 0, -2, -3}, {4, 3, 0, -3, -4},
						{3, 2, 0, -2, -3},{2, 1, 0, -1, -2}};
const double sobelY5[5][5] = {{2, 3, 4, 3, 2},{1, 2, 3, 2, 1}, {0, 0, 0, 0, 0},
						{-1, -2, -3, -2, -1},{-2, -3, -4, -3, -2}};

// 9*9 sobel operator
const double sobelX9[9][9] = {{4, 3, 2, 1, 0, -1, -2, -3, -4},
						{5, 4, 3, 2, 0, -2, -3, -4, -5}, 
						{6, 5, 4, 3, 0, -3, -4, -5, -6},
						{7, 6, 5, 4, 0, -4, -5, -6, -7},
						{8, 7, 6, 5, 0, -5, -6, -7, -8},
						{7, 6, 5, 4, 0, -4, -5, -6, -7}, 
						{6, 5, 4, 3, 0, -3, -4, -5, -6},
						{5, 4, 3, 2, 0, -2, -3, -4, -5},
						{4, 3, 2, 1, 0, -1, -2, -3, -4}};

const double sobelY9[9][9] = {{4, 5, 6, 7, 8, 7, 6, 5, 4},
						{3, 4, 5, 6, 7, 6, 5, 4, 3}, 
						{2, 3, 4, 5, 6, 5, 4, 3, 2},
						{1, 2, 3, 4, 5, 4, 3, 2, 1},
						{0, 0, 0, 0, 0, 0, 0, 0, 0},
						{-1, -2, -3, -4, -5, -4, -3, -2, -1}, 
						{-2, -3, -4, -5, -6, -5, -4, -3, -2},
						{-3, -4, -5, -6, -7, -6, -5, -4, -3},
						{-4, -5, -6, -7, -8, -7, -6, -5, -4}};

// the number of missing pixel
int missingNum = 0;

using namespace std;

ImageInpainting::ImageInpainting(void)
{
}

ImageInpainting::ImageInpainting(ColorImage img)
{
	targImg = img;

	width = img.getWidth();
	height = img.getHeight();
	imgPixels = img.getPixels();
}

// labeling the source region, the front region of missing region and missing region
void ImageInpainting::findTarget(PixelRGB rgb, PixelRGB rgb2, bool sourceFlag)
{
	Pixel *pixels, tmpPixel, neighbourPixel;
	PixelRGB tmpRgb, stdRgb;
	double per = 0;
	int i, j, h, k, size, pos;
	stdRgb = PixelRGB();
	stdRgb.r = 255;
	stdRgb.g = 0;
	stdRgb.b = 0;

	size = width * height;
	pixels = imgPixels;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			pos = i * width + j;
			tmpPixel = pixels[pos];
			tmpRgb = tmpPixel.getRGB();
			// missing region

			if (typeid(tmpPixel.getFlag()) == typeid(int) && tmpPixel.getFlag() >= 0)
			{
				// 2 is missing region, 3 is the front of target region
				if (tmpPixel.getFlag() == 2 || tmpPixel.getFlag() == 3)
				{
					missingNum++;
				}
				continue;
			}
			
			if (Common::colorRange(tmpRgb, rgb, rgb2))
			{
				// missing region
				tmpPixel.setFlag(2);
				tmpPixel.setRGB(stdRgb);
				missingNum++;
				
			} else if(sourceFlag)
			{
				// source region
				tmpPixel.setFlag(1);
			}
			pixels[pos] = tmpPixel;
		}
	}
	per = missingNum * 100.0 / (width * height);
	cout<<"the percent of missing region: "<<per<<"%"<<endl;

	contourPixel *tmpNode, *start;
	int prePos = 0, currentPos = 0, currentX = 0, currentY = 0,  type = 0;
	int missingRegionEndX, missingRegionEndY;
	bool isOver = false;
	missingRegionEndX = missingRegionEndY = 0;

	// 0403 add
	contourHead = new contourPixel;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			pos = i * width + j;
			tmpPixel = pixels[pos];
			tmpRgb = tmpPixel.getRGB();
			// missing region
			if (tmpPixel.getFlag() == 2)
			{
				// judge border 
				// * * *
				// * @ 4
				// 1 2 3
				//

				// 04/03 delete
				// contourHead = new contourPixel;
				contourHead->pixel = tmpPixel;
				contourHead->x = j;
				contourHead->y = i;
				contourHead->nextPixel = NULL;
				contourHead->head = true;
				start = contourHead;
				if (Common::valueRange(i + 1, 0, height))
				{
					if (Common::valueRange(j - 1, 0, width) && pixels[(i + 1) * width + j - 1].getFlag() == 2)
					{
						currentX = j - 1;
						currentY = i + 1;
						type = 2;
					} else if (pixels[(i + 1) * width + j].getFlag() == 2)
					{
						currentX = j;
						currentY = i + 1;
						type = 3;
					} else if (Common::valueRange(j + 1, 0, width) && pixels[(i + 1) * width + j + 1].getFlag() == 2)
					{
						currentX = j + 1;
						currentY = i + 1;
						type = 3;
					} else if (Common::valueRange(j + 1, 0, width) && pixels[i * width + j + 1].getFlag() == 2)
					{
						currentX = j + 1;
						currentY = i;
						type = 4;
					} else
					{
						continue;
					}
				} else
				{
					continue;
				}
				
				while(currentPos != pos && !isOver)
				{
					// record border pixel
					currentPos = currentY * width + currentX;
					pixels[currentPos].setFlag(3);
					tmpNode = new contourPixel;
					tmpNode->x = currentX;
					tmpNode->y = currentY;
					tmpNode->pixel = pixels[currentPos];
					contourHead->nextPixel = tmpNode;
					tmpNode = contourHead;

					switch (type)
					{
					case 1:
						//調査点は前回の追跡点の下、右下の場合、以下の順番で調べる
						// type = 1
						// 3 2 1
						// 4 @ 8
						// 5 6 7
						if (Common::valueRange(currentY - 1, 0, height))
						{
							if (Common::valueRange(currentX + 1, 0, width) && pixels[(currentY - 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 4;
								currentX++;
								currentY--;
								break;
							} else if (pixels[(currentY - 1) * width + currentX].getFlag() == 2)
							{
								type = 1;
								currentY--;
								break;
							} else if (Common::valueRange(currentX - 1, 0, width) && pixels[(currentY - 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 1;
								currentX--;
								currentY--;
								break;
							}
						} 
						
						if (Common::valueRange(currentX - 1, 0, width) && pixels[currentY * width + currentX - 1].getFlag() == 2)
						{
							type = 2;
							currentX--;
							break;
						}
						
						if (Common::valueRange(currentY + 1, 0, height))
						{
							if (Common::valueRange(currentX - 1, 0, width) && pixels[(currentY + 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 2;
								currentX--;
								currentY++;
								break;
							} else if (pixels[(currentY + 1) * width + currentX].getFlag() == 2)
							{
								type = 3;
								currentY++;
								break;
							} else if (Common::valueRange(currentX + 1, 0, width) && pixels[(currentY + 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 3;
								currentX++;
								currentY++;
								break;
							}
						}
						
						if (Common::valueRange(currentX + 1, 0, width) && pixels[currentY * width + currentX + 1].getFlag() == 2)
						{
							type = 4;
							currentX++;
						} else {
							isOver = true;
						}
						break;
					case 2:
						//調査点は前回の追跡点の右、右上の場合、以下の順番で調べる
						// type = 2
						// 1 8 7
						// 2 @ 6
						// 3 4 5
						if (Common::valueRange(currentX - 1, 0, width))
						{
							if (Common::valueRange(currentY - 1, 0, height) && pixels[(currentY - 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 1;
								currentX--;
								currentY--;
								break;
							} else if (pixels[currentY * width + currentX - 1].getFlag() == 2)
							{
								type = 2;
								currentX--;
								break;
							} else if (Common::valueRange(currentY + 1, 0, height) && pixels[(currentY + 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 2;
								currentX--;
								currentY++;
								break;
							}
						}
						
						if (Common::valueRange(currentY + 1, 0, height) && pixels[(currentY + 1) * width + currentX].getFlag() == 2)
						{
							type = 3;
							currentY++;
							break;
						}
						
						if (Common::valueRange(currentX + 1, 0, width))
						{
							if (Common::valueRange(currentY + 1, 0, height) && pixels[(currentY + 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 3;
								currentX++;
								currentY++;
								break;
							} else if (pixels[currentY * width + currentX + 1].getFlag() == 2)
							{
								type = 4;
								currentX++;
								break;
							} else if (Common::valueRange(currentY - 1, 0, height) && pixels[(currentY - 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 4;
								currentX++;
								currentY--;
								break;
							}
						}
						
						if (Common::valueRange(currentY - 1, 0, height) && pixels[(currentY - 1) * width + currentX].getFlag() == 2)
						{
							type = 1;
							currentY--;
						} else {
							isOver = true;
						}
						break;
					case 3:
						//調査点は前回の追跡点の上、左上の場合、以下の順番で調べる
						// type = 3
						// 7 6 5
						// 8 @ 4
						// 1 2 3
						if (Common::valueRange(currentY + 1, 0, height))
						{
							if (Common::valueRange(currentX - 1, 0, width) && pixels[(currentY + 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 2;
								currentX--;
								currentY++;
								break;
							} else if (pixels[(currentY + 1) * width + currentX].getFlag() == 2)
							{
								type = 3;
								currentY++;
								break;
							} else if (Common::valueRange(currentX + 1, 0, width) && pixels[(currentY + 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 3;
								currentX++;
								currentY++;
								break;
							}
						}
						
						if (Common::valueRange(currentX + 1, 0, width) && pixels[currentY * width + currentX + 1].getFlag() == 2)
						{
							type = 4;
							currentX++;
							break;
						}
						
						if (Common::valueRange(currentY - 1, 0, height))
						{
							if (Common::valueRange(currentX + 1, 0, width) && pixels[(currentY - 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 4;
								currentX++;
								currentY--;
								break;
							} else if (pixels[(currentY - 1) * width + currentX].getFlag() == 2)
							{
								type = 1;
								currentY--;
								break;
							} else if (Common::valueRange(currentX - 1, 0, width) && pixels[(currentY - 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 1;
								currentX--;
								currentY--;
								break;
							}
						}
						
						if (Common::valueRange(currentX - 1, 0, width) && pixels[currentY * width + currentX - 1].getFlag() == 2)
						{
							type = 2;
							currentX--;
						} else {
							isOver = true;
						}
						break;
					case 4:
						//調査点は前回の追跡点の左、左下の場合、以下の順番で調べる
						// type = 4
						// 5 4 3
						// 6 @ 2
						// 7 8 1
						if (Common::valueRange(currentX + 1, 0, width))
						{
							if (Common::valueRange(currentY + 1, 0, height) && pixels[(currentY + 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 3;
								currentX++;
								currentY++;
								break;
							} else if (pixels[currentY * width + currentX + 1].getFlag() == 2)
							{
								type = 4;
								currentX++;
								break;
							} else if (Common::valueRange(currentY - 1, 0, height) && pixels[(currentY - 1) * width + currentX + 1].getFlag() == 2)
							{
								type = 4;
								currentX++;
								currentY--;
								break;
							}
						}
						
						if (Common::valueRange(currentY - 1, 0, height) && pixels[(currentY - 1) * width + currentX].getFlag() == 2)
						{
							type = 1;
							currentY--;
							break;
						} 
						
						if (Common::valueRange(currentX - 1, 0, width))
						{
							if (Common::valueRange(currentY - 1, 0, height) && pixels[(currentY - 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 1;
								currentX--;
								currentY--;
								break;
							} else if (pixels[currentY * width + currentX - 1].getFlag() == 2)
							{
								type = 2;
								currentX--;
								break;
							} else if (Common::valueRange(currentY + 1, 0, height) && pixels[(currentY + 1) * width + currentX - 1].getFlag() == 2)
							{
								type = 2;
								currentX--;
								currentY++;
								break;
							}
						}
						
						if (Common::valueRange(currentY + 1, 0, height) && pixels[(currentY + 1) * width + currentX].getFlag() == 2)
						{
							type = 3;
							currentY++;
						} else {
							isOver = true;
						}
						break;
					default:
						break;
					}
				}
				contourHead->tail = true;
				contourHead->nextPixel = start;

				// 20160403 add
				delete contourHead;
				return;
			}
		}
	}
}

void ImageInpainting::displayTarget(string fileName)
{
	ColorImage res;
	Pixel *pixels, *resPixels;
	PixelRGB rgb_r, rgb_g, rgb_b, rgb_o;
	int i, j, h, k, size;

	size = width * height;
	pixels = targImg.getPixels();
	resPixels = new Pixel[size];

	res.setPixels(resPixels);
	res.setWidthHeight(width, height);
 
	rgb_r.r = 255;
	rgb_r.g = 255;
	rgb_r.b = 0;

	rgb_g.r = 255;
	rgb_g.g = 0;
	rgb_g.b = 0;

	rgb_b.r = 0;
	rgb_b.g = 0;
	rgb_b.b = 255;

	rgb_o.r = 100;
	rgb_o.g = 100;
	rgb_o.b = 100;

	for (i = 0; i < size; i++)
	{
		if (pixels[i].getFlag() == 1)
		{
			resPixels[i] = Pixel(rgb_r);
		} else if (pixels[i].getFlag() == 2)
		{
			resPixels[i] = Pixel(rgb_g);
			
		} else if (pixels[i].getFlag() == 3)
		{
			resPixels[i] = Pixel(rgb_b);
		} else
		{
			resPixels[i] = Pixel(rgb_o);
		}
	}

	res.saveImage(fileName);
	delete []resPixels;
}

Pixel * ImageInpainting::getSimilarPatch(int targPosX, int targPosY)
{
	Pixel *resPixels, *patchPixels, defaultPixel;
	PixelLab targLab, refLab;
	double ssd, dif, minSSD = INFINITY;
	int i, j, h, k, patchPosX, patchPosY, indexPos, countPixel, _height, _width, patch_width, patch_half_width, patch_s;
	bool isOver = false, invalidRegion = false;
	defaultPixel.setFlag(4);
	
	patchPosX = patchPosY = 0;
	patch_width = PATCH_SIZE;
	patch_half_width = patch_width >> 1;
	patch_s = patch_width * patch_width;

	_height = height - patch_width;
	_width = width - patch_width;

	patchPixels = new Pixel[patch_width * patch_width];

	for (h = 0; h < patch_width; ++h)
	{
		for (k = 0; k < patch_width; ++k)
		{
			if (targPosY + h - patch_half_width < 0 || targPosY + h - patch_half_width >= height
				|| k + targPosX - patch_half_width < 0 || k + targPosX - patch_half_width >= width)
			{
				patchPixels[h * patch_width + k] = defaultPixel;
			}
			else
			{
				patchPixels[h * patch_width + k] = imgPixels[(targPosY + h - patch_half_width) * width + k + targPosX - patch_half_width];
			}
		}
	}
	
	for (i = 0; i < _height; ++i)
	{
		for (j = 0; j < _width; ++j)
		{
			ssd = 0.0;
			countPixel = 0;
			for (h = 0; h < patch_width; ++h)
			{
				for (k = 0; k < patch_width; ++k)
				{
					// check if the pixel is in source area or not.
					// the patch can not be referred if missing pixel is included in the patch.
					
					if (imgPixels[(i + h) * width + j + k].getFlag() != 1)
					{
						invalidRegion = true;
						goto breakPoint1;
					}
					
					// indexPos = (targPosY + h - patch_half_width) * width + k + targPosX - patch_half_width;
					if (patchPixels[h * patch_width + k].getFlag() != 1) {
						continue;
					}
					
					// computing SSD by using CIELab color space
					targLab = patchPixels[h * patch_width + k].getLab();
					refLab = imgPixels[(i + h) * width + j + k].getLab();
					
					dif = targLab.L - refLab.L;
					ssd += dif * dif;

					dif = targLab.a - refLab.a;
					ssd += dif * dif;

					dif = targLab.b - refLab.b;
					ssd += dif * dif;

					countPixel++;
				}
			}
			
			breakPoint1:
			if (invalidRegion)
			{
				invalidRegion = false;
				continue;
			}
			
			ssd /= countPixel;

			if (ssd < minSSD)
			{
				minSSD = ssd;
				patchPosX = j;
				patchPosY = i;

				if (minSSD < 30) {
					goto breakPoint2;
				}
			}
		}
	}

	breakPoint2:
	
	delete[]patchPixels;

//	 get the patch most similar to targImg from refImg
	resPixels = new Pixel[patch_s];

	for (h = 0; h < patch_width; ++h)
	{
		for (k = 0; k < patch_width; ++k)
		{
			resPixels[h * patch_width + k] = imgPixels[(patchPosY + h) * width + patchPosX + k];
		}
	}

	return resPixels;
}

void ImageInpainting::initConfidence()
{
	
	Pixel *pixels = imgPixels;
	Pixel tmpPixel;
	int i, size;

	size = width * height;

	for (i = 0; i < size; i++)
	{
		tmpPixel = pixels[i];
		if (tmpPixel.getFlag() == 1)
		{
			tmpPixel.setConfidence(1);
		} else
		{
			tmpPixel.setConfidence(0);
		}

		pixels[i] = tmpPixel;
	}
}

void ImageInpainting::displayPriority(string fileName)
{
	ColorImage resImg;
	Pixel tmpP, *pixels;
	double minPri, maxPri, *tmpData;
	int i, j, currPos;
	unsigned char tmpH = 0;
	
	pixels = targImg.getPixels();
	minPri = INFINITY;
	maxPri = 0.0;
	currPos = 0;
	tmpData = new double[width * height];

	Pixel *pixelsRes = new Pixel[width * height]();
	PixelRGB pRGB;
	pRGB.r = pRGB.g = pRGB.b = 0;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			currPos = i * width + j;
			tmpP = pixels[currPos];
			if (tmpP.getFlag() != 3)
			{
				tmpData[currPos] = 0.0;
				continue;
			}
			tmpData[currPos] = tmpP.getPriority();
			if (tmpData[currPos] > maxPri)
			{
				maxPri = tmpData[currPos];
			}

			if (tmpData[currPos] < minPri)
			{
				minPri = tmpData[currPos];
			}
		}
	}

	

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			currPos = i * width + j;
			if (tmpData[currPos] == 0)
			{
				tmpData[currPos] = minPri;
			}
			tmpH = (unsigned char)((tmpData[currPos] - minPri) * 255 / (maxPri - minPri));
			if (tmpH == '\0')
			{
				tmpH = 1;
			}
			pRGB.r = pRGB.g = pRGB.b = tmpH;
			
			pixelsRes[currPos].setRGB(pRGB);
		}
	}

	resImg.setWidthHeight(width, height);
	resImg.setPixels(pixelsRes);
	resImg.saveImage(fileName);

	delete []pixelsRes;
	delete []tmpData;
}

void ImageInpainting::computePriority()
{
	Pixel *tmpPixels, *tmpPixel, defaultPixel;
	double priority, confidence;
	int i, j, h, k, patch_width_half;
	
	defaultPixel = Pixel();
	defaultPixel.setFlag(4);
	patch_width_half = PATCH_SIZE >> 1;

	tmpPixels = new Pixel[PATCH_SIZE * PATCH_SIZE];

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			tmpPixel = &imgPixels[i * width + j];

			double confidence = 0.0;
			int countPixel = 0;

			if (tmpPixel->getFlag() == 3)
			{
				
				for (h = 0; h < PATCH_SIZE; h++)
				{
					for (k = 0; k < PATCH_SIZE; k++)
					{
						if (Common::valueRange(h + i - patch_width_half, 0, height - 1) && Common::valueRange(j + k - patch_width_half, 0, width - 1))
						{
							confidence += imgPixels[(h + i - patch_width_half) * width + j + k - patch_width_half].getConfidence();
							countPixel++;
						}
					}
				}
				
				confidence /= countPixel;
				priority = confidence * computeData(j, i, imgPixels);

				tmpPixel->setPriority(priority);
				tmpPixel->setConfidence(confidence);

				//imgPixels[pos] = tmpPixel;
			}
		}
	}

	delete[]tmpPixels;
	tmpPixels = NULL;
}

double ImageInpainting::computeConfidence(Pixel* patchPixels)
{
	double confidence = 0.0;
	int patch_s = PATCH_SIZE * PATCH_SIZE;

	for (int i = 0; i < patch_s; i++)
	{
		if (patchPixels[i].getFlag() == 4)
		{
			continue;
		}
		confidence += patchPixels[i].getConfidence();
	}

	return confidence /= patch_s;
}

void ImageInpainting::displayConfidence(string fileName)
{
	GrayImage resImg;
	Pixel tmpP, *pixels;
	double minCon, maxCon, *tmpData;
	int i, j, currPos;
	unsigned char *resData;
	

	pixels = targImg.getPixels();
	minCon = INFINITY;
	maxCon = 0.0;
	currPos = 0;
	resData = new unsigned char[width * height];
	tmpData = new double[width * height];

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			currPos = i * width + j;
			tmpP = pixels[currPos];

			tmpData[currPos] = tmpP.getConfidence();
			if (tmpData[currPos] > maxCon)
			{
				maxCon = tmpData[currPos];
			}

			if (tmpData[currPos] < minCon)
			{
				minCon = tmpData[currPos];
			}
		}
	}

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			currPos = i * width + j;
			if (tmpData[currPos] == 0)
			{
				tmpData[currPos] = minCon;
			}
			resData[currPos] = (unsigned char)((tmpData[currPos] - minCon) * 255 / (maxCon - minCon));
			if (resData[currPos] == '\0')
			{
				resData[currPos] = 1;
			}
		}
	}

	resImg = GrayImage();
	resImg.setWidthHeight(width, height);
	resImg.setImageData(resData);
	resImg.saveImage(fileName);

	delete []tmpData;
}

// compute isophote for RGB
isophote ImageInpainting::computeIsophoteForRGB(int x, int y, ColorImage img, int rgb)
{
	Pixel *pixels;
	isophote resIso;
	double sX, sY;
	int i, j, patchW, patchH, frontX, frontY, vectorX, vectorY, currPos;
	unsigned char *grayData;
	bool invalid = false;

	patchW = img.getWidth();
	patchH = img.getHeight();

	grayData = new unsigned char[patchW * patchH];

	switch (rgb)
	{
	case 0:
		grayData = img.getImageDataR();
		break;
	case 1:
		grayData = img.getImageDataG();
		break;
	case 2:
		grayData = img.getImageDataB();
		break;
	default:
		break;
	}

	pixels = img.getPixels();

	sX = sY = 0.0;

	for (i = -1; i < 2; i++)
	{
		for (j = -1; j < 2; j++)
		{
			currPos = (i + y) * patchW + j + x;

			if (!Common::valueRange(i + y, 0, patchH) 
				|| !Common::valueRange(j + x, 0, patchW))
			{
				invalid = true;
				sX = 0;
				sY = 0;
				break;
			}

			sX += grayData[(i + y) * patchW + j + x] * sobelX[i + 1][j + 1];
			sY += grayData[(i + y) * patchW + j + x] * sobelY[i + 1][j + 1];

		}
		if (invalid)
		{
			break;
		}
	}

	resIso.intensity = sqrt(sX * sX + sY * sY);
	resIso.intensityX = sX;
	resIso.intensityY = sY;

	if (sX == 0)
	{
		resIso.direction = HALF_PI;
	}if (sY == 0)
	{
		resIso.direction = 0;
	} else
	{
		resIso.direction = atan(sY / sX);
	}

	delete []grayData;
	return resIso;
}

/*
* compute the isophote of pixel.
*@param {x, y}  the coordinate of pixel
*@param {type}  the type of pixel.(0:every type, 1: source region, 2: missing region, 
*                                  3: the front region of missing region, 4: error region(beyond the border of image))
*/
isophote ImageInpainting::computeIsophote(int x, int y, int type)
{
	isophote resIso;
	double gray, tmpR, tmpG, tmpB, sX, sY;
	int i, j, frontX, frontY, vectorX, vectorY, currPos;
	unsigned char *patchR, *patchG, *patchB;
	bool invalid = false;

	patchR = targImg.getImageDataR();
	patchG = targImg.getImageDataG();
	patchB = targImg.getImageDataB();

	sX = sY = 0.0;

	for (i = -1; i < 2; i++)
	{
		for (j = -1; j < 2; j++)
		{
			currPos = (i + y) * width + j + x;

			if (!Common::valueRange(i + y, 0, height)
				|| !Common::valueRange(j + x, 0, width))
			{
				invalid = true;
				sX = 0;
				sY = 0;
				break;
			}

			// set isophote as zero if any pixel of missing region are in the target patch 
			// when computing the isophote of the source region
			if (type == 1 && imgPixels[currPos].getFlag() != 1)
			{
				invalid = true;
				sX = 0;
				sY = 0;
				break;
			//	continue;
			}

			gray = 0.298912 * patchR[currPos] + 0.586611 * patchG[currPos] + 0.114478 * patchB[currPos];

			//tmpR = pow(patchR[currPos], 2.2) * 0.222015;
			//tmpG = pow(patchG[currPos], 2.2) * 0.706655;
			//tmpB = pow(patchB[currPos], 2.2) * 0.071330;
			//gray = pow(tmpR + tmpG + tmpB, 1.0 / 2.2);

			sX += gray * sobelX[i + 1][j + 1];
			sY += gray * sobelY[i + 1][j + 1];

		}
		if (invalid)
		{
			break;
		}
	}

	//sX /= 3;
	//sY /= 3;

	resIso.intensity = sqrt(sX * sX + sY * sY);
	resIso.intensityX = sX;
	resIso.intensityY = sY;

	if (sX == 0)
	{
		resIso.direction = HALF_PI;
	}
	
	if (sY == 0)
	{
		resIso.direction = 0;
	} else
	{
		resIso.direction = atan(sY / sX);
	}
//	resIso.direction = abs(resIso.direction);
	patchR = NULL;
	patchG = NULL;
	patchB = NULL;

	return resIso;
}

void ImageInpainting::displayIsophote(string fileName)
{
	GrayImage resImg;
	Pixel *pixels;
	isophote tmpIso;
	double *data, min, max;
	int width, height, i, j;
	unsigned char *resData;
	
	pixels = targImg.getPixels();
	min = INFINITY;
	max = 0.0;

	width = targImg.getWidth();
	height = targImg.getHeight();
	resData = new unsigned char[width * height];
	data = new double[width * height];

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (pixels[i * width + j].getFlag() != 1)
			{
				continue;
			}
			
			tmpIso = computeIsophote(j, i, 1);
			data[i * width + j] = tmpIso.intensity;
			if (tmpIso.intensity > max)
			{
				max = tmpIso.intensity;
			}

			if (tmpIso.intensity < min)
			{
				min = tmpIso.intensity;
			}
		}
	}

	for (i = 0; i < width * height; i++)
	{
		resData[i] = (unsigned char)((data[i] - min) * 255 / (max - min));
		
		if (resData[i] == '\0')
		{
			resData[i] = 1;
		}
	}

	resImg = GrayImage();
	resImg.setImageData(resData);
	resImg.setWidthHeight(width, height);

	resImg.saveImage(fileName);

	delete []data;
	delete[]resData;
}

void ImageInpainting::setAvalibleRegion()
{
	int i, j, _width, _height, h, k, patch_width;
	bool isAvalible;

	_width = width - PATCH_SIZE;
	_height = height - PATCH_SIZE;
	patch_width = PATCH_SIZE;
	for (i = 0; i < _height; ++i)
	{
		for (j = 0; j < _width; ++j)
		{
			if (imgPixels[i * width + j].getIsAvalible())
			{
				continue;
			}
			isAvalible = true;
			for (h = 0; h < patch_width; ++h)
			{
				for (k = 0; k < patch_width; ++k)
				{
					// check if the pixel is in source area or not.
					// the patch can not be referred if missing pixel is included in the patch.
					if (imgPixels[(i + h) * width + j + k].getFlag() != 1)
					{
						isAvalible = false;
					}
				}
			}
			imgPixels[i * width + j].setIsAvalible(isAvalible);
		}
	}
}

double ImageInpainting::computeTangent(int posX, int posY, int type)
{
	Pixel *pixels;
	PixelRGB tmpRgb;
	double sX, sY, direc, *tmpPixelValue;
	int i, j, h, k, patch_size, rangeComp, tmpIndex;

	patch_size = PATCH_SIZE;
	rangeComp = patch_size >> 1;
	pixels = imgPixels;
	tmpPixelValue = new double[patch_size * patch_size];
	
	sX = sY = direc = 0.0;

	for (i = -rangeComp; i < rangeComp + 1; i++)
	{
		for (j = -rangeComp; j < rangeComp + 1; j++)
		{

			if (Common::valueRange(i + posY, 0, height) &&
				Common::valueRange(j + posX, 0, width) &&
				pixels[(i + posY) * width + j + posX].getFlag() == 1)
			{
				//tmpRgb = pixels[(i + posY) * patchW + j + posX].getRGB();
				//gray = 0.298912 * tmpRgb.r + 0.586611 * tmpRgb.g + 0.114478 * tmpRgb.b;
			//	tmpPixelValue[(i + 4) * 9 + j + 4] = gray * gauKern[i + 4][j + 4] / 273.0;
				tmpPixelValue[(i + rangeComp) * patch_size + j + rangeComp] = 100;
			} else
			{
				tmpPixelValue[(i + rangeComp) * patch_size + j + rangeComp] = 0;
			}
			
		}
	}

//	double sobel_x, *sobel_y;
	/*
	if (PATCH_SIZE == 5)
	{
		for (i = -rangeComp; i < rangeComp + 1; i++)
		{
			for (j = -rangeComp; j < rangeComp + 1; j++)
			{
				sX += tmpPixelValue[(i + rangeComp) * PATCH_SIZE + j + rangeComp] * sobelX5[i + rangeComp][j + rangeComp];
				sY += tmpPixelValue[(i + rangeComp) * PATCH_SIZE + j + rangeComp] * sobelY5[i + rangeComp][j + rangeComp];
			}
		}
	} else 
	if (PATCH_SIZE == 9)
	{
		for (i = -rangeComp; i < rangeComp + 1; i++)
		{
			for (j = -rangeComp; j < rangeComp + 1; j++)
			{
				sX += tmpPixelValue[(i + rangeComp) * PATCH_SIZE + j + rangeComp] * sobelX9[i + rangeComp][j + rangeComp];
				sY += tmpPixelValue[(i + rangeComp) * PATCH_SIZE + j + rangeComp] * sobelY9[i + rangeComp][j + rangeComp];
			}
		}
	}
	*/
	for (i = -rangeComp; i < rangeComp + 1; ++i)
	{
		for (j = -rangeComp; j < rangeComp + 1; ++j)
		{
			tmpIndex = (i + rangeComp) * patch_size + j + rangeComp;
			sX += tmpPixelValue[tmpIndex] * sobelX9[i + rangeComp][j + rangeComp];
			sY += tmpPixelValue[tmpIndex] * sobelY9[i + rangeComp][j + rangeComp];
		}
	}



	if (sX == 0)
	{
		direc = HALF_PI;
	}
	
	if (sY == 0)
	{
		direc = 0;
	} else
	{
		direc = atan(sY / sX);
	}

	delete[]tmpPixelValue;
	return direc;
}

/*
*@param {poxX}  x value of the target pixel
*@param {poxY}  y value of the target pixel
*@param {patch} the image or image patch where the target pixel is
*/
double ImageInpainting::computeData(int posX, int posY, Pixel *pixels)
{
	isophote isoIp, maxIsoIp;
	double Ip, res, tmpP, tagentP, Np;
	double maxRes = 0.0;
	int i, j, currPos, tmpX, tmpY, count = 1;
	int rangeComp = PATCH_SIZE >> 1;
	int rangeComp_max = rangeComp;

	tagentP = computeTangent(posX, posY, 3);

	if (tagentP < 0)
	{
		Np = tagentP + HALF_PI;
	} else
	{
		Np = tagentP - HALF_PI;
	}
	
	maxIsoIp.intensityX = maxIsoIp.intensityY = maxIsoIp.intensity = maxIsoIp.direction = 0;

	// set the maximum isophote as isophote of target p from pixels of patch centering on target pixel
	for (i = -rangeComp; i < rangeComp_max; i++)
	{
		for (j = -rangeComp; j < rangeComp_max; j++)
		{
			tmpX = j + posX;
			tmpY = i + posY;
			currPos = (tmpY) * width + tmpX;

			if (Common::valueRange(tmpY, 0, height) && Common::valueRange(tmpX, 0, width) && pixels[currPos].getFlag() == 1)
			{
				isoIp = computeIsophote(tmpX, tmpY, 1);
				res = isoIp.intensity * abs(cos(Np - isoIp.direction));
			//	res = isoIp.intensity;
				if (res >= maxRes)
				{
					maxRes = res;
					maxIsoIp = isoIp;
				}
			}
		}
	}
	// normalize isophote
	maxRes = maxRes / 255.0;
	return maxRes;
}

// the color of missing region is defined between rgb1 and rgb2
void ImageInpainting::inpainting(PixelRGB rgb1, PixelRGB rgb2, bool hasLabeling)
{
	// initializing
	if (!hasLabeling)
	{
		findTarget(rgb1, rgb2, true);
	}

	initConfidence();
	computePriority();
	targImg.rgbToLab();

	Pixel tmpPixel, neighPixel;
	Pixel *similarPatchPixels;
	PixelRGB defaultRGB;

	double priority = 0.0;
	double targPixelConf;
	double maxPriority = 0.0;

	int _patch_size = PATCH_SIZE;
	int _patch_size_half = _patch_size >> 1;
	int updateRegionSize = _patch_size_half + 1;
	int updateRegionSizeR = updateRegionSize + 1;
	int updateRegionSizeS = updateRegionSize * updateRegionSize;
	int tmpX, tmpY;
	int count = 0;
	int tmpPos;
	int i, j, h, k, pos;
	int targPosX, targPosY;

	bool isOver = false;

	do
	{
		maxPriority = 0.0;
		targPosX = targPosY = 0;
		isOver = true;
		// search the border pixel with highest priority
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				tmpPixel = imgPixels[i * width + j];
				if (tmpPixel.getFlag() == 3)
				{
					// the border pixel is found
					priority = tmpPixel.getPriority();
					if (maxPriority <= priority)
					{
						maxPriority = priority;
						targPosX = j;
						targPosY = i;
					}
					isOver = false;
				}
			}
		}

		if (isOver)
		{
			break;
		}

		similarPatchPixels = getSimilarPatch(targPosX, targPosY);
		targPixelConf = imgPixels[targPosY * width + targPosX].getConfidence();

		for (h = 0; h < _patch_size; h++)
		{
			for (k = 0; k < _patch_size; k++)
			{
				tmpPos = (targPosY + h - _patch_size_half) * width + targPosX + k - _patch_size_half;
				if (Common::valueRange(targPosY + h - _patch_size_half, 0, height)
					&& Common::valueRange(targPosX + k - _patch_size_half, 0, width)
					&& imgPixels[tmpPos].getFlag() != 1)
				{
					imgPixels[tmpPos] = similarPatchPixels[h * _patch_size + k];
					
					imgPixels[tmpPos].setConfidence(targPixelConf);
					imgPixels[tmpPos].setFlag(1);
					count++;
				}			
			}
		}
		
		delete []similarPatchPixels;
		// update the the border of missing region.
		for (h = -updateRegionSize; h < updateRegionSizeR; h++)
		{
			for (k = -updateRegionSize; k < updateRegionSizeR; k++)
			{
				tmpX = targPosX + k;
				tmpY = targPosY + h;
				if (Common::valueRange(tmpY, 0, height)
					&& Common::valueRange(tmpX, 0, width) && abs(h) * abs(k) != updateRegionSizeS
					&& (abs(h) == updateRegionSize || abs(k) == updateRegionSize) 
					&& imgPixels[(tmpY) * width + tmpX].getFlag() == 2)
				{
					imgPixels[(tmpY) * width + tmpX].setFlag(3);
				}
			}
		}

		cout << "percentage of completeness｣ｺ " << 100.0 * count / missingNum<< "%" << endl;

		computePriority();
	} while (true);

	targImg.saveImage("images/inpainting/inpaintingRes.ppm");
}

ImageInpainting::~ImageInpainting(void)
{
}
