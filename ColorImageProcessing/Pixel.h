#pragma once

#include<map>

using namespace std;

struct PixelRGB {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct PixelXYZ {
	double x;
	double y;
	double z;
};

struct PixelYUV {
	double Y;
	double U;
	double V;
};

struct PixelCIRLuv {
	double L;
	double u;
	double v;
};

struct PixelLab {
	double L;
	double a;
	double b;
};

struct PixelPos {
	int x;
	int y;
};

struct isophote
{
	double direction;
	double intensity;
	double intensityX;
	double intensityY;
};

class Pixel
{
private:
	int color;
	PixelPos pos;

	PixelRGB rgb;
	PixelLab Lab;
  
	/*
	1: source region
	2: target region
	3: the front of target region
	4: default value
	5: object region
	6: edge region
	7: reference region
	8: missing object region
	*/
	int flag;

	/*
	1: object
	2: edge of object
	3: missing region of object
	4: edge of the missing region of object
	*/

	bool isAvalible;
	int obFlag;
	double grayValue;

	double dist;

	isophote isoPixel;

	int completedFlag;

	double confidence;
	double priority;
	double computeLabTools_1(double t);
	double computeLabTools_2(double t);

	// parameters for coloring of EBCUO 
	double luminance;
	PixelCIRLuv luv;
	double stdDevi;
	double *gabor;
	int gaborDim;
	double avgGabor;
	double avgDeviationOfNeig;
	double scale;
	double *descriptor;
	double orientation;
	int segmengtNov;
	double feature;

	int corresPosX;
	int corresPosY;
	int corresSegNo;

	unsigned char *surfFeature;
	//  ---------------------------
public:
	Pixel(void);
	Pixel(int color, int confidence, int priority);
	Pixel(PixelRGB rgbValue, int confidence, int priority);
	Pixel(PixelRGB rgbValue, PixelPos pos);
	Pixel(PixelRGB rgbValue);

	double minDis;

	void setIsAvalible(bool f);
	bool getIsAvalible();

	void setCompletedFlag(int value);
	int getCompletedFlag();

	void setIsophote(isophote iso);
	isophote getIsophote();

	void setColor(int color);
	int getColor();

	void setGrayValue(double value);
	void setGrayValue();
	double getGrayValue();

	void setConfidence(double confidence);
	double getConfidence();

	void setPriority(double priority);
	double getPriority();

	void setFlag(int flag);
	int getFlag();

	void setObFlag(int obF);
	int getObFlag();

	void setPos(PixelPos pos);
	PixelPos getPos();

	void setRGB(PixelRGB pRgb);
	PixelRGB getRGB();

	PixelLab rgbTOLab(PixelRGB rgb);
	PixelLab getLab();

	PixelRGB normalizeRGB(PixelRGB rgb);
	PixelXYZ rgbTOxyz(PixelRGB rgb);
	PixelLab xyzTOLab(PixelXYZ xyz);
	PixelRGB xyzTOrgb(PixelXYZ xyz);
	PixelRGB LabTOrgb(PixelLab lab);
	PixelXYZ LabTOxyz(PixelLab lab);

	PixelYUV rgbToYUV(PixelRGB rgb);
	PixelRGB YUVTorgb(PixelYUV yuv);
	
	// -------- for coloring of EUCUO -----------
	double GetDenominator(PixelXYZ xyz);
	PixelCIRLuv rgbToLuv(PixelRGB rgb);
	PixelCIRLuv getLuv();

	void initPram();
	double getStdDevi();

	void setStdDevi(double devi);
	void initGabor(int dim);
	double getAvgGabor();
	void setGabor(double g, int index);
	int getGaborDim();
	double *getGabors();

	double getIpixelScale();
	double *getDescriptor();
	double getOrientation();

	void setSegmentNov(int nov);
	int getSegmentNov();

	void setFeature(double f);
	double getFeature();

	void setSurfFeature(unsigned char *f);
	unsigned char *getSurfFeature();

	void setCorresPos(int x, int y);
	int getCorresPosX();
	int getCorresPosY();

	void setCorresSegNo(int n);
	int getCorresSegNo();

	void setDistance(double dist);
	double getDistance();

	// -----------------------------------------

	~Pixel(void);
};

