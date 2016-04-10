//�J���[�摜�����p�̃w�b�_�t�@�C��
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define _CRT_SECURE_NO_WARNINGS

// �萔�錾
#define MAX_IMAGESIZE	3000 //�z�肷�鉡�E�c�̍ő��f��
#define MAX_BRIGHTENNESS	255 //�z�肷��ő�K���l
#define GRAYLEVEL	256 //�z�肷��K����
#define MAX_FILENAME 256 //���̓t�@�C�����̍ő咷
#define MAX_BUFFERSIZE 256 //���p����o�b�t�@�ő咷
#define MAX_NUM_OF_IMAGES 3	//���p����摜�̖���


// ���ϐ��̐錾
unsigned char image[MAX_NUM_OF_IMAGES][MAX_IMAGESIZE][MAX_IMAGESIZE][3];
int width[MAX_NUM_OF_IMAGES], height[MAX_NUM_OF_IMAGES];

// �֐��̃v���g�^�C�v�錾
void load_color_image(int n, char name[]);
void save_color_image(int n, char name[]);
void copy_color_image(int n1, int n2);
void init_color_image(int n, int red, int green, int blue);

void load_color_image(int n, char name[]) 
{
	char fname[MAX_FILENAME];
	char buffer[MAX_BUFFERSIZE];
	FILE *fp;
	int max_gray=0, x, y, col;

	if (name[0] == '\0')
	{
		printf("���̓t�@�C���� (*.ppm) : ");
        scanf("%s",fname);
	} else {
		strcpy(fname, name);
	}

	if((fp=fopen(fname, "rb")) == NULL) {
		printf("���̓t�@�C���G���[");
		exit(1);
	}

	fgets(buffer, MAX_BUFFERSIZE, fp);
	if (buffer[0] != 'P' || buffer[1] != '6')
	{
		printf("�t�@�C���t�H�[�}�b�g�G���[");
		exit(1);
	}

	width[n] = 0;
	height[n] = 0;
	while (width[n] == 0 || height[n] == 0)
	{
		fgets(buffer, MAX_BUFFERSIZE, fp);
		if (buffer[0] != '#')
		{
			sscanf(buffer, "%d %d", &width[n], &height[n]);
		}
	}

	while (max_gray == 0)
	{
		fgets(buffer, MAX_BUFFERSIZE, fp);
		if (buffer[0] != '#')
		{
			sscanf(buffer, "%d", &max_gray);
		}
	}

	// �摜�̃p�����[�^��\������
	printf("���F%d,�@�c�F%d\n", width[n], height[n]);
	printf("�ő�K������%d\n", max_gray);

	if (width[n] > MAX_IMAGESIZE || height[n] > MAX_IMAGESIZE || max_gray != MAX_BRIGHTENNESS)
	{
		printf("�ő�K���l���s�K�؂ł�\n");
		exit(1);
	}

	for (y = 0; y < height[n]; y++)
	{
		for ( x = 0; x < width[n]; x++)
		{
			for (col = 0; col < 3; col++)
			{
				image[n][x][y][col] = (unsigned char)fgetc(fp);
			}
		}
	}

	printf("�J���[�摜���摜No,%d�ɓǂݍ��݂܂���. \n", n);
	fclose(fp);
}


void save_color_image(int n, char name[]){
	char fname[MAX_FILENAME];
	FILE *fp;
	int x, y, col;

	if (name[0] == '\0')
	{
		printf("�o�̓t�@�C�����i�g���q��ppm�j: ");
		scanf("%s", fname);
	} else {
		strcpy(fname, name);
	}

	if ((fp = fopen(fname, "wb")) == NULL)
	{
		printf("unable to open file");
		exit(1);
	}

	fputs("P6\n", fp);
	fputs("# Created by image Processing\n", fp);
	fprintf(fp, "%d %d\n", width[n], height[n]);
	fprintf(fp, "%d\n", MAX_BRIGHTENNESS);

	for (y = 0; y < height[n]; y++)
	{
		for ( x = 0; x < width[n]; x++)
		{
			for (col = 0; col < 3; col++)
			{
				fputc(image[n][x][y][col], fp);
			}
		}
	}

	fclose(fp);
	printf("�摜�͐���ɏo�͂���܂����B");
};

void copy_color_image(int n1, int n2){
	int x, y, col;

	width[n2] = width[n1];
	height[n2] = height[n1];

	for (y = 0; y < height[n1]; y++)
	{
		for ( x = 0; x < width[n1]; x++)
		{
			for (col = 0; col < 3; col++)
			{
				image[n2][x][y][col] = image[n1][x][y][col];
			}
		}
	}
};

void init_color_image(int n, int red, int green, int blue){
	int x, y;

	for (y = 0; y < height[n]; y++)
	{
		for ( x = 0; x < width[n]; x++)
		{
			if (red != -1)
			{
				image[n][x][y][0] = red;
			}
			
			if (green != -1)
			{
				image[n][x][y][1] = green;
			}
			
			if (blue != -1)
			{
				image[n][x][y][2] = blue;
			}
		}
	}
};

//void createhistgram(int n){
//	int histR[256], histG[256], histB[256];
//	int x, y, col, maxR, maxG, maxB;
//	maxR = maxG = maxB = 0;
//
//	for (y = 0; y < height[n]; y++)
//	{
//		for ( x = 0; x < width[n]; x++)
//		{
//			histR[image[n][x][y][0]]++;
//			histR[image[n][x][y][1]]++;
//			histR[image[n][x][y][2]]++;
//		}
//	}
//
//	for (int i = 0; i < 256; i++)
//	{
//		if (maxR < histR[i])
//		{
//			maxR = histR[i];
//		}
//
//		if (maxG < histG[i])
//		{
//			maxG = histG[i];
//		}
//
//		if (maxB < histB[i])
//		{
//			maxB = histB[i];
//		}
//	}
//
//	// output histgram images
//	FILE *fpR, *fpG, *fpB;
//
//	if ((fpR = fopen("histGramR", "wb")) == NULL)
//	{
//		printf("unable to open file");
//		exit(1);
//	}
//	fputs("P6\n", fpR);
//	fputs("# Created by image Processing\n", fpR);
//	fprintf(fpR, "256 256\n");
//	fprintf(fpR, "%d\n", MAX_BRIGHTENNESS);
//
//
//	for (x = 0; x < 256; x++)
//	{
//		for (y = 0; y < 256; y++)
//		{
//
//		}
//	}
//
//	if ((fpG = fopen("histGramG", "wb")) == NULL)
//	{
//		printf("unable to open file");
//		exit(1);
//	}
//	fputs("P6\n", fpG);
//	fputs("# Created by image Processing\n", fpG);
//	fprintf(fpG, "256 256\n");
//	fprintf(fpG, "%d\n", MAX_BRIGHTENNESS);
//
//	if ((fpB = fopen("histGramB", "wb")) == NULL)
//	{
//		printf("unable to open file");
//		exit(1);
//	}
//	fputs("P6\n", fpB);
//	fputs("# Created by image Processing\n", fpB);
//	fprintf(fpB, "256 256\n");
//	fprintf(fpB, "%d\n", MAX_BRIGHTENNESS);
//};