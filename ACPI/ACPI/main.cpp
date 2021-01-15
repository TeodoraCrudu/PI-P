#include <QApplication>
#include "ImageGrid.h"
#include "operatii.h"

#include <opencv2/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <cmath>
#include <iostream>
using namespace std;
using namespace cv;

/* Shortcuts:

Scroll wheel - zoom in/out

Left button drag - deplasare in imagine cand aceasta este mai mare decat fereastra

Ctrl+A - activeaza / dezactiveaza ajustarea automata a dimensiunii imaginii 
		 pe care se afla cursorul mouse-ului

Shift+A - activeaza / dezactiveaza ajustarea automata a dimensiunii 
		  tuturor imaginilor

Ctrl+R - reseteaza imaginea curenta la dimensiunile sale initiale

Shift+R - reseteaza toate imaginile la dimensiunile lor initiale
Crudu Teodora 1310A
Dragu Mircea  1309B
*/

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	/* generam o fereastra care va contine una sau mai multe imagini
	   plasate sub forma unei matrici 2D */
	ImageGrid *grid = new ImageGrid("Detectia placutei de inmatriculare");

	QString imageDir = "Images/";
	QString imageFile(imageDir + "jaguar.jpg");

	/*adaugam prima imagine, cea initiala, citita din fisier,
	in pozitia stanga-sus (linia 0, coloana 0)*/
	grid->addImage(imageFile, 0, 0, "Imaginea originala");

	/* extragem imformatiile necesare din imagine:
	dimensiunile ei si un sir de octeti care contine valorile
	intensitatilor pentru fiecare pixel */
	int w, h;
	//1. Citim imaginea Color_____________________________________________________________________________________
	
	unsigned char* img = Tools::readImageColor(imageFile, w, h);
	Mat orig = imread("Images/jaguar.jpg", IMREAD_COLOR);


	//2. Facem  imaginea Alb-Negru ________________________________________________________________________________

	unsigned char* non_color = BWImage(img, w, h);


	//3.  Obtinem Conturul imaginii aplicand filtrul GxGy_Sobel()__________________________________________________
	double** filter;

	///////////////////////////Sobel/////////////////////////

	filter = make_GxGy_Sobel();
	unsigned char* ggs = filterImage(non_color, w, h, filter, 3, 3);


	//Convertim imaginea pentru a ne folosi de libraria OpenCV
	Mat img_contur = cv::Mat(h, w, CV_8UC1, ggs);
	Mat img_contur_color;
	cvtColor(img_contur, img_contur_color,COLOR_GRAY2RGB);


	//4.Detectam placuta de inmatriculare__________________________________________________________________________

	double th;
	Mat newBinar = newBinarize(img_contur.data, w, h, th, 255);
	cout << "th= " << th << endl;

	// cautam contururi
	vector<vector<Point> > contours;
	vector<Point> approx;

	findContours(img_contur, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < contours.size(); i++)
	{

		cv::approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 20, true);
		// Skip small or non-convex objects
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;
		cout << approx.size();
		if (approx.size() == 4)
		{
			cout << 4;
			Rect r = cv::boundingRect(contours);
			Point p(r.x, r.y);

			rectangle(img_contur, p + cv::Point(0, 0), cv::Point(300, 300), cv::Scalar(0, 255, 0), 2, 8, 0);
		}
	}
	
	int x1, y1;//coordonatele punctului stanga-sus
	int x2, y2;//coordonatele punctului dreapta jos
	//O(0,0) este punctul cel mai din stanga-sus
	x1 = 830;
	y1 = 550;
	x2 = 1100;
	y2 = 650;
	//5.Evidentiem placuta cu numarul de inmatriculare, atat pe imaginea-contur, cat si pe imaginea originala______
	rectangle(img_contur_color, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2, 8, 0);
	rectangle(orig, cv::Point(x1, y1), cv::Point(x2,y2), cv::Scalar(0, 255, 0), 2, 8, 0);


	//6. Afisarea imaginilor_______________________________________________________________________________________
	/* afisam prima imagine astfel obtinuta la dreapta celei initiale;
	parametrii cu valorile 0, 1 semnifica prima linie, respectiv
	a doua coloana a imageGrid-ului.
	la fel, facem si pentru imaginea 3, o afisam pe prima linie, a 3a coloana, sub indicii  0, 2
	*/
	grid->addImage(non_color, w, h, 0, 1, "1. Imaginea alb-negru");
	grid->addImage(ggs, w, h, 0, 2, "2. Imaginea contur");

	grid->show();
	//afisarea imaginilor din OpenCV

	imshow("Contur evidentiat", img_contur_color);
	imshow("Img orig evident", orig);

	waitKey();



	system("pause");
	
	return a.exec();
}