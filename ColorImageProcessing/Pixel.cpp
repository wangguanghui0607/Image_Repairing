#include "stdafx.h"
#include "Pixel.h"
#include <math.h>
#include<iostream>

using namespace std;

PixelXYZ whiteRef = {95.047, 100.000, 108.883};

Pixel::Pixel(void)
{
	priority = 0;
	// surf 128
	surfFeature = new unsigned char[128];
	grayValue = NULL;
	isAvalible = false;
	initPram();
}

Pixel::Pixel(int col, int con, int pri)
{
	color = col;
	confidence = con;
	priority = pri;
	grayValue = NULL;
}

Pixel::Pixel(PixelRGB rgbValue, PixelPos p)
{
	rgb = rgbValue;
	pos = p;
	grayValue = NULL;
}

Pixel::Pixel(PixelRGB rgbValue)
{
	rgb = rgbValue;
	grayValue = NULL;
}

void Pixel::setDistance(double d)
{
	dist = d;
}

double Pixel::getDistance()
{
	return dist;
}

void Pixel::setIsAvalible(bool f)
{
	isAvalible = f;
}

bool Pixel::getIsAvalible()
{
	return isAvalible;
}

void Pixel::setCompletedFlag(int value)
{
	completedFlag = value;
}

int Pixel::getCompletedFlag()
{
	return completedFlag;
}

void Pixel::setIsophote(isophote iso)
{
	isoPixel = iso;
}

isophote Pixel::getIsophote()
{
	return isoPixel;
}

void Pixel::setColor(int col)
{
	color = col;
}

int Pixel::getColor()
{
	return color;
}

void Pixel::setGrayValue(double value)
{
	grayValue = value;
}

void Pixel::setGrayValue()
{
	grayValue = 0.298912 * rgb.r + 0.586611 * rgb.g + 0.114478 * rgb.b;
}

double Pixel::getGrayValue()
{
	setGrayValue();
	return grayValue;
}

void Pixel::setConfidence(double con)
{
	confidence = con;
}

double Pixel::getConfidence()
{
	return confidence;
}

void Pixel::setPriority(double pri)
{
	priority = pri;
}

double Pixel::getPriority()
{
	return priority;
}

void Pixel::setObFlag(int obF)
{
	obFlag = obF;
}

int Pixel::getObFlag()
{
	return obFlag;
}

void Pixel::setFlag(int f)
{
	flag = f;
}

int Pixel::getFlag()
{
	return flag;
}

void Pixel::setPos(PixelPos p)
{
	pos = p;
}

PixelPos Pixel::getPos()
{
	return pos;
}

void Pixel::setRGB(PixelRGB pRgb)
{
	rgb = pRgb;
}

PixelRGB Pixel::getRGB()
{
	return rgb;
}

PixelXYZ Pixel::rgbTOxyz(PixelRGB rgb)
{
	PixelXYZ xyz;
	double R, G, B, sR, sG, sB;
	double tmp;

	sR = rgb.r / 255.0;
	sG = rgb.g / 255.0;
	sB = rgb.b / 255.0;


	// gamma correction
	R = computeLabTools_2(sR);
	G = computeLabTools_2(sG);
	B = computeLabTools_2(sB);

	/*
	‚w =0.3933‚q  + 0.3651‚f + 0.1903‚a 
	‚x =0.2123‚q + 0.7010‚f + 0.0858‚a@@@@@@@@Ž®i‚Qj 
	‚y = 0.0182‚q + 0.1117‚f + 0.9570‚a

	    _x = _r * 0.4124 + _g * 0.3576 + _b * 0.1805;  
        _y = _r * 0.2126 + _g * 0.7152 + _b * 0.0722;  
        _z = _r * 0.0193 + _g * 0.1192 + _b * 0.9505;  
	*/
	xyz.x = (0.4124 * R + 0.3576 * G + 0.1805 * B) * 100;
	xyz.y = (0.2126 * R + 0.7152 * G + 0.0722 * B) * 100;
	xyz.z = (0.0193 * R + 0.1192 * G + 0.9505 * B) * 100;

	return xyz;
}

PixelLab Pixel::getLab()
{
	// return (Lab.L) ? Lab : rgbTOLab(rgb);
	return Lab;
}

double Pixel::computeLabTools_1(double t)
{
	double res, param, paramPow;
	paramPow = 1.0 / 3.0;
	param = pow(6.0 / 29.0, 3);

	if (t > param)
	{
		res = pow(t, paramPow);
	} else
	{
		res = pow(29.0 / 6.0, 2) * t / 3.0 + 4.0 / 29.0;
	}

	return res;
}

double Pixel::computeLabTools_2(double t)
{
	double res, param, paramPow;
	param = 0.04045;
	paramPow = 2.4;

	if (t > param)
	{
		res = pow((t + 0.055) / 1.055, paramPow);
	} else
	{
		res = t / 12.92;
	}

	return res;
}


PixelLab Pixel::rgbTOLab(PixelRGB rgb)
{
	PixelXYZ xyz;
	double Xn, Yn, Zn, paramPow, tmpY, tmpX, tmpZ;
	double param, tX, tY, tZ;
	paramPow = 1.0 / 3.0;
	param = pow(6.0 / 29.0, paramPow);

	//Xn = 98.072;
	//Yn = 100.0;
	//Zn = 118.225;

	Xn = 95.0456;
	Yn = 100.0;
	Zn = 108.8754;
	xyz = rgbTOxyz(rgb);

	tX = xyz.x / Xn;
	tY = xyz.y / Yn;
	tZ = xyz.z / Zn;
	

	tmpX = computeLabTools_1(tX);
	tmpY = computeLabTools_1(tY);
	tmpZ = computeLabTools_1(tZ);

	Lab.L = 116.0 * tmpY - 16.0;

	Lab.a = 500.0 * (tmpX - tmpY);
	Lab.b = 200.0 * (tmpY - tmpZ);

	Lab.L = (int)Lab.L * 255 / 100;
	Lab.a = (int)Lab.a;
	Lab.b = (int)Lab.b;
	//cout<<"rgb:  "<<(int)rgb.r<<", "<<(int)rgb.g<<", "<<(int)rgb.b<<endl;
//	cout<<"Lab:  "<<Lab.L<<", "<<Lab.a<<", "<<Lab.b<<endl;
	return Lab;
}

PixelYUV Pixel::rgbToYUV(PixelRGB rgb)
{
	/*
	Y =  0.299R + 0.587G + 0.114B  (0~255)
	U = -0.169R - 0.331G + 0.500B  (-128~127)
	V =  0.500R - 0.419G - 0.081B  (-128~127)
	*/
	PixelYUV yuv;
	yuv.Y = 0.299 * rgb.r + 0.587 * rgb.g + 0.114 * rgb.b;
	yuv.U = -0.169 * rgb.r - 0.331 * rgb.g + 0.500 * rgb.b;
	yuv.V = 0.500 * rgb.r - 0.419 * rgb.g - 0.081 * rgb.b;

	return yuv;
}

PixelRGB Pixel::YUVTorgb(PixelYUV yuv)
{
	/*
	R = 1.000Y          + 1.402V
	G = 1.000Y - 0.344U - 0.714V
	B = 1.000Y + 1.772U
	*/
	PixelRGB rgb;
	rgb.r = 1.000 * yuv.Y + 1.402 * yuv.V;
	rgb.g = 1.000 * yuv.Y - 0.344 * yuv.U - 0.714 * yuv.V;
	rgb.b = 1.000 * yuv.Y + 1.772 * yuv.U;

	return rgb;
}

PixelCIRLuv Pixel::rgbToLuv(PixelRGB rgb)
{
	
	double L, u, v;
	PixelCIRLuv luv;
	PixelXYZ xyz;

	xyz = rgbTOxyz(rgb);

	/*
	‚b‚h‚d‚ª‚P‚X‚V‚U”N‚É‚k*‚*‚‚*F‹óŠÔ‚Æ‚Æ‚à‚É’è‚ß‚½‹Ï“™F‹óŠÔDŽŸŽ®‚Å’è‹`‚³‚ê‚éD
	L*=116(‚x^‚xn)O1/3-16 ‚x^‚x‚Ž„‚OD‚O‚O‚W‚W‚T‚U 
	u*=13L*(u'-u'n) 
	v*=13L*(v'-v'n) 
	‚±‚±‚ÅC‚x’l‚Í”CˆÓ‚ÈF‚Ì‚w‚x‚y•\FŒn‚É‚¨‚¯‚éŽOŽhŒƒ’l‚Ì‚x’lC u'Cv'‚Í‚b‚h‚d1976UCS F“xÀ•W‚Å‚ ‚éD
	‚xnCu'nCv'n‚Í•W€‚ÌŒõ‚Ì‰º‚É‚¨‚¯‚éŠ®‘SŠgŽU”½ŽË–Ê‚É‘Î‚·‚é‚x’lCu'Cv' F“xÀ•W‚Å‚ ‚èC
	‚x‚Ž‚P‚O‚O‚Æ’è‚ß‚ç‚ê‚Ä‚¢‚éD
	un' = 0.2009, vn' = 0.4610

	u' = 4X / (X + 15Y + 3Z) = 4x / ( -2x + 12y + 3 )
    v' = 9Y / (X + 15Y + 3Z) = 9y / ( -2x + 12y + 3 ).
	*/

	double targetDenomi = GetDenominator(xyz);
	double refDenomi = GetDenominator(whiteRef);

	// ‚x^‚xn = Yn_, un', vn'
	double Yn_ = xyz.y/whiteRef.y;
	double un_, vn_, u_, v_;
	double Epsilon = 0.008856;

	if (Yn_ > Epsilon)
	{
		L = 116.0 * pow( Yn_, 1.0 / 3.0) - 16.0;
	} else
	{
		L = Yn_ * 903.3;
	}

	u = (targetDenomi == 0) ? 0 : 13.0 * L * (4.0 * xyz.x / targetDenomi - 4.0 * whiteRef.x / refDenomi);
	v = (targetDenomi == 0) ? 0 : 13.0 * L * (9.0 * xyz.y / targetDenomi - 9.0 * whiteRef.y / refDenomi);

	luv.L = L;
	luv.u = u;
	luv.v = v;

	return luv;
}

double Pixel::GetDenominator(PixelXYZ xyz)
{
	return xyz.x + 15.0 * xyz.y + 3.0 * xyz.z;
}

PixelCIRLuv Pixel::getLuv()
{
	if (luv.L == NULL)
	{
		luv = rgbToLuv(getRGB());
	}

	return luv;
}

void Pixel::initPram()
{
	luminance = 0;
	luv.L = 0.0;
	luv.u = 0.0;
	luv.v = 0.0;

	surfFeature = 0;

	stdDevi = 0.0;
	avgDeviationOfNeig = 0.0;
}

double Pixel::getStdDevi()
{
	return stdDevi;
}

void Pixel::setStdDevi(double devi)
{
	stdDevi = devi;
}

void Pixel::initGabor(int dim)
{
	gaborDim = dim;
	gabor = new double[gaborDim];

	for (int i = 0; i < gaborDim; i++)
	{
		gabor[i] = 0.0;
	}
}

void Pixel::setGabor(double g, int index)
{
	if (index < 0 || index >= gaborDim)
	{
		index = 0;
	}
	gabor[index] = g;
}

double Pixel::getAvgGabor()
{
	double total = 0.0;

	for (int i = 0; i < gaborDim; i++)
	{
		total += gabor[i];
	}

	avgGabor = total / gaborDim;
	return avgGabor;
}

double *Pixel::getGabors()
{
	return gabor;
}

int Pixel::getGaborDim()
{
	return gaborDim;
}

void Pixel::setSegmentNov(int nov)
{
	segmengtNov = nov;
}

int Pixel::getSegmentNov()
{
	return segmengtNov;
}

void Pixel::setFeature(double f)
{
	feature = f;
}

double Pixel::getFeature()
{
	return feature;
}

void Pixel::setSurfFeature(unsigned char *f)
{
	surfFeature = f;
}

void Pixel::setCorresPos(int x, int y)
{
	corresPosX = x;
	corresPosY = y;
}

int Pixel::getCorresPosX()
{
	return corresPosX;
}

int Pixel::getCorresPosY()
{
	return corresPosY;
}

void Pixel::setCorresSegNo(int n)
{
	corresSegNo = n;
}

int Pixel::getCorresSegNo()
{
	return corresSegNo;
}

unsigned char *Pixel::getSurfFeature()
{
	return surfFeature;
}

Pixel::~Pixel(void)
{
}
