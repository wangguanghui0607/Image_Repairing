// ラベリング用ヘッダファイル
#include "ppmlib.h"

int label[MAX_IMAGESIZE][MAX_IMAGESIZE];

int labeling(int n, int red, int green, int blue);

int search_4neighbors(int x, int y, int n);

void modify_label(int num1, int num2, int n);
void save_labeledImage(int n);

void modify_label(int num1, int num2, int n) {
	int x, y;

	for (y = 0; y < height[n]; y++)
	{
		for ( x = 0; x < width[n]; x++)
		{
			if (label[x][y] == num1)
			{
				label[x][y] = num2;
			}
		}
	}
}

int search_4neighbors(int x, int y, int n){
	int max = 0;

	if (y - 1 >= 0 && label[x][y - 1] > max)
	{
		max = label[x][y - 1]; // top
	}

	if (y + 1 <= height[n] && label[x][y + 1] > max)
	{
		max = label[x][y + 1]; // buttom
	}

	if (x - 1 >= 0 && label[x - 1][y] > max)
	{
		max = label[x - 1][y]; // right
	}

	if (x + 1 <= width[n] && label[x + 1][y] > max)
	{
		max = label[x + 1][y]; // right
	}

	return max;
}

int labeling(int n, int red, int green, int blue){
	int x, y, num;
	int count = 0;
	int new_count;

	// init
	for (y = 0; y < height[n]; y++)
	{
		for ( x = 0; x < width[n]; x++)
		{
			label[x][y] = 0;
		}
	}

	// scan image
	for (y = 0; y < height[n]; y++)
	{
		for ( x = 0; x < width[n]; x++)
		{
			if ((image[n][x][y][0] != red || image[n][x][y][1] != green || image[n][x][y][2] != blue) && label[x][y] == 0)
			{
				num = search_4neighbors(x, y, n);
				if (num == 0)
				{
					label[x][y] = ++count;
				} else
				{
					label[x][y] = num;
				}
			}
		}
	}

	if (count > 0)
	{
		for (y = 0; y < height[n]; y++)
		{
			for ( x = 0; x < width[n]; x++)
			{
				if (label[x][y] != 0)
				{
					num = search_4neighbors(x, y, n);
					if (num > label[x][y])
					{
						modify_label(num, label[x][y], n);
					}
				}
			}
		}

		new_count = 0;
		for (y = 0; y < height[n]; y++)
		{
			for ( x = 0; x < width[n]; x++)
			{
				if (label[x][y] > new_count)
				{
					new_count++;
					modify_label(label[x][y], new_count, n);
				}
			}
		}

		return new_count;
	}
	else
	{
		return 0;
	}
}

void save_labeledImage(int n2){
	int x, y;

	int changedLabel[MAX_IMAGESIZE][MAX_IMAGESIZE];

	//width[n1] = width[n2];
	//height[n1] = height[n2];

	for (y = 0; y < height[n2]; y++)
	{
		for ( x = 0; x < width[n2]; x++)
		{
			changedLabel[x][y] = 0;
			//image[n2][x][y][0] = image[n1][x][y][0];
			//image[n2][x][y][1] = image[n1][x][y][1];
			//image[n2][x][y][2] = image[n1][x][y][2];
		}
	}

	for (y = 0; y < height[n2]; y++)
	{
		for ( x = 0; x < width[n2]; x++)
		{
			if (changedLabel[x][y] == 0)
			{
				for (int i = -1; i < 2; i++)
				{
					for (int j = -1; j < 2; j++)
					{
						if (x + i >= 0 && y + j >=0 && x + i <= width[n2] && y + j <= height[n2] &&
							changedLabel[x + i][y + j] == 0 && label[x][y] != label[x + i][y + j])
						{
							image[n2][x][y][0] = image[n2][x + i][y + j][0] = 255;
							image[n2][x][y][1] = image[n2][x + i][y + j][1] = 0;
							image[n2][x][y][2] = image[n2][x + i][y + j][2] = 0;

							changedLabel[x][y] = changedLabel[x + i][y + j] = 1;
							break;
						}
					}
					if (changedLabel[x][y] != 0)
					{
						break;
					}
				}
			}
		}
	}
}