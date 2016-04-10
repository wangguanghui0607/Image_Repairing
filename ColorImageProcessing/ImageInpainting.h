#pragma once
#include "ColorImage.h"
#include "Pixel.h"

struct contourPixel
{
	Pixel pixel;
	double priority;
	bool head;
	bool tail;
	int x;
	int y;
	contourPixel *nextPixel;
	contourPixel *prePixel;
};

class ImageInpainting
{
protected:
	ColorImage targImg;
	contourPixel *contourHead;


public:
	ImageInpainting(void);

	ImageInpainting(ColorImage img);

	int width;
	int height;
	Pixel *imgPixels;

	void labelingPixel(ColorImage img);

	void findTarget(PixelRGB rgb1, PixelRGB rgb2,  bool sourceFlag);
	void findTargetBorder(PixelRGB rgb);
	void displayTarget(string fileName);

	isophote computeIsophote(int x, int y, int type);
	void displayIsophote(string fileName);

	void setAvalibleRegion();

	// compute isophote for RGB
	// rgb: 0  red
	// rgb: 1 green
	// rgb: 2 blue
	isophote computeIsophoteForRGB(int x, int y, ColorImage img, int rgb);

	// compute tangent of contour of missing region at pixel p
	double computeTangent(int x, int y, int type);

	Pixel* getSimilarPatch(int targPosX, int targPosY);

	// Object Removal by Exemplar-Based Inpainting by A.Criminisi etc.
	// initializate the confidence value of pixel
	void initConfidence();
	// computing patch priorities
	void computePriority();
	void displayPriority(string fileName);
	// computing the confidence term
	double computeConfidence(Pixel *patchPixels);
	void displayConfidence(string fileName);
	// computing the data term
	double computeData(int posX, int posY, Pixel *patchPixels);
	// ---------------------------------------------------------------
	// Inpainting
	// Region Filling and Object Removal by Exemplar-Based Image Inpainting by Criminisi .etc
	void inpainting(PixelRGB rgb, PixelRGB rgb2, bool hasLabeling);

	// Image Completion with Structure Propagation by Jian Sun .etc
	void inpaintingStructurePropa(PixelRGB rgb, PixelRGB rgb2);
	~ImageInpainting(void);
};

