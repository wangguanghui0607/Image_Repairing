// ColorImageProcessing.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include "ColorImage.h"
#include "GrayImage.h"
#include "ImageInpainting.h"
#include "Common.h"
#include<iostream>
#include <hash_map>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <cv.h>
#include <highgui.h>
using namespace cv;

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	ColorImage trgImg("images/inpainting/brick03.ppm");

	ImageInpainting *inpaint = new ImageInpainting(trgImg);

	PixelRGB rgb1, rgb2;
	// test02
	rgb1.r = 255;
	rgb1.g = 10;
	rgb1.b = 10;

	rgb2.r = 250;
	rgb2.g = 0;
	rgb2.b = 0;

	inpaint->inpainting(rgb1, rgb2, false, "images / inpainting / inpaintingRes.ppm");

	delete inpaint;
	return 0;
}
