#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <cmath>

using namespace cv;

unsigned char* negateImage(unsigned char* img, int w, int h)
{
	unsigned char* result = new unsigned char[w*h];
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
			result[y*w + x] = 255 - img[y*w + x];
	return result;
}

//Functie de convertire din color un alb-negru
unsigned char* BWImage(unsigned char* img, int w, int h)
{
	unsigned char* result = new unsigned char[w * h];
	for (int i = 0; i < h; i++)
		for (int j = 0; j < 3 * w; j += 3)
		{
			result[i * w + j/3] = (img[i * 3 *w + j]+ img[i * 3 * w + j + 1] + img[i * 3 * w + j + 2]) / 3;
		}
	delete img;
	return result;
}

// aloca memorie pentru o matrice de dimensiune m x n
double** aloca2D(int m, int n)
{
	double** rez = new double* [m];
	for (int i = 0; i < m; ++i)
	{
		rez[i] = new double[n];
	}
	return rez;
}

// dealoca memoria unei matrici cu m linii
void dealoca2D(double** A, int m)
{
	for (int i = 0; i < m; ++i)
	{
		if (A[i])
		{
			delete[] A[i];
		}
	}
	if (A)
	{
		delete[] A;
	}
}

// aplica un filtru de dimensiune fw x fh pe o imagine
unsigned char* filterImage(unsigned char* img, int w, int h, double** filter, int fw, int fh)
{
	unsigned char* result = new unsigned char[w * h];

	for (int y = 0; y < h; ++y)
	{
		for (int x = 0; x < w; ++x)
		{
			// facem produsul de convolutie pentru fiecare pixel (x, y) in parte
			double sum = 0;
			for (int i = 0; i < fw; ++i)
			{
				for (int j = 0; j < fh; ++j)
				{
					// daca suntem pe margini, aplicam filtrul doar partial
					if ((y - fh / 2 + i >= 0) && (y - fh / 2 + i < h) && (x - fw / 2 + j) >= 0 && (x - fw / 2 + j < w))
					{
						sum += filter[i][j] * img[(y - fh / 2 + i) * w + x - fw / 2 + j];
					}
				}
			}
			double finalPixel = sum;
			// corectam pixelul final rezultat, pentru a nu fi in afara intervalului [0, 255]
			if (finalPixel > 255)
			{
				finalPixel = 255;
			}
			else if (finalPixel < 0)
			{
				finalPixel = 0;
			}
			result[y * w + x] = (unsigned char)finalPixel;
		}
	}

	return result;
}

double** make_horizontal_Sobel()
{
	double** x = aloca2D(3, 3);

	x[0][0] = x[0][2] = -1;
	x[0][1] = -2;
	x[1][0] = x[1][1] = x[1][2] = 0;
	x[2][0] = x[2][2] = 1;
	x[2][1] = 2;

	return x;
}

double** make_vertical_Sobel()
{
	double** x = aloca2D(3, 3);

	x[0][0] = x[2][0] = -1;
	x[1][0] = -2;
	x[0][1] = x[1][1] = x[2][1] = 0;
	x[0][2] = x[2][2] = 1;
	x[1][2] = 2;

	return x;
}

double** make_GxGy_Sobel()
{
	double** x = aloca2D(3, 3);

	x[0][0] = -2;
	x[2][2] = 2;
	x[0][1] = x[1][0] = -1;
	x[2][1] = x[1][2] = 1;
	x[2][0] = x[1][1] = x[0][2] = 0;

	return x;
}

double** make_minus_GxGy_Sobel()
{
	double** x = aloca2D(3, 3);

	x[0][0] = x[1][1] = x[2][2] = 0;
	x[0][1] = x[1][2] = -1;
	x[0][2] = -2;
	x[1][0] = x[2][1] = 1;
	x[2][0] = 2;

	return x;
}

unsigned char* binarizeImage(unsigned char* img, int w, int h, double th, double maxVal)
{
	unsigned char* result = new unsigned char[w * h];
	cv::Mat inMat(h, w, CV_8UC1, img);
	cv::Mat binaryMat(h, w, CV_8UC1, result);
	cv::threshold(inMat, binaryMat, th, maxVal, cv::THRESH_BINARY);
	return result;
}


cv::Mat newBinarize(unsigned char* img, int w, int h, double& th, double maxVal)
{
	unsigned char* result = new unsigned char[w * h];
	cv::Mat inMat(h, w, CV_8UC1, img);
	cv::Mat binaryMat(h, w, CV_8UC1, result);

	th = cv::threshold(inMat, binaryMat, 80, maxVal, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);
	return  binaryMat;

}
