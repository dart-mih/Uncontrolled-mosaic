#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string> 
#include <ctime>

// Включить заголовочные файлы из каталога OpenCV
// требуется для сшивания изображений.
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"


using namespace std;

using namespace cv;

void counting(string name_img1, string name_img2, int* dh, int* dw, int* max_h, ofstream& pozition_result)
{
	unsigned int start_time;
	unsigned int finish_time;
	Mat img1 = imread(name_img1);
	Mat img2 = imread(name_img2);
	if (img1.cols > (*max_h))
		(*max_h) = img1.cols;
	if (img2.cols > (*max_h))
		(*max_h) = img2.cols;
	int count = 0;
	int maxcount = 0;
	*dh = 0;
	*dw = -100;
	int row;
	int col;
	if (img1.rows < img2.rows)
		row = img1.rows;
	else
		row = img2.rows;
	if (img1.cols < img2.cols)
		col = img1.cols;
	else
		col = img2.cols;
	int chanels = img1.channels();
	uchar* p10;
	uchar* p11; 
	uchar* p12;
	uchar* p20;
	uchar* p21;
	uchar* p22;
	start_time = clock();
	for (int k = row / 4; k < 2 * row / 5; k += 3)
	{
		count = 0;
		for (int i = 0; i < row - 3; i += 3)
		{
			if (k + i + 3 >= row)
			{
				break;
			}
			p10 = img1.ptr<uchar>(k + i);
			p11 = img1.ptr<uchar>(k + i + 1);
			p12 = img1.ptr<uchar>(k + i + 2);
			p20 = img2.ptr<uchar>(i);
			p21 = img2.ptr<uchar>(i + 1);
			p22 = img2.ptr<uchar>(i + 2);

			for (int j = 0; j < col * chanels; j += 3 * chanels)
			{
				if (j + 3 >= col * chanels)
				{
					break;
				}
				int dl1 = 0;
				int dl2 = 0;
				int img10 = (p10[dl1 + j] + p11[dl1 + j] + p12[dl1 + j] +
					p10[dl1 + j + 3] + p11[dl1 + j + 3] + p12[dl1 + j + 3] +
					p10[dl1 + j + 6] + p11[dl1 + j + 6] + p12[dl1 + j + 6]) / 9;
				int img11 = (p10[dl1 + j + 1] + p11[dl1 + j + 1] + p12[dl1 + j + 1] +
					p10[dl1 + j + 4] + p11[dl1 + j + 4] + p12[dl1 + j + 4] +
					p10[dl1 + j + 7] + p11[dl1 + j + 7] + p12[dl1 + j + 7]) / 9;
				int img12 = (p10[dl1 + j + 2] + p11[dl1 + j + 2] + p12[dl1 + j + 2] +
					p10[dl1 + j + 5] + p11[dl1 + j + 5] + p12[dl1 + j + 5] +
					p10[dl1 + j + 8] + p11[dl1 + j + 8] + p12[dl1 + j + 8]) / 9;
				int img20 = (p20[dl2 + j] + p21[dl2 + j] + p22[dl2 + j] +
					p20[dl2 + j + 3] + p21[dl2 + j + 3] + p22[dl2 + j + 3] +
					p20[dl2 + j + 6] + p21[dl2 + j + 6] + p22[dl2 + j + 6]) / 9;
				int img21 = (p20[dl2 + j + 1] + p21[dl2 + j + 1] + p22[dl2 + j + 1] +
					p20[dl2 + j + 4] + p21[dl2 + j + 4] + p22[dl2 + j + 4] +
					p20[dl2 + j + 7] + p21[dl2 + j + 7] + p22[dl2 + j + 7]) / 9;
				int img22 = (p20[dl2 + j + 2] + p21[dl2 + j + 2] + p22[dl2 + j + 2] +
					p20[dl2 + j + 5] + p21[dl2 + j + 5] + p22[dl2 + j + 5] +
					p20[dl2 + j + 8] + p21[dl2 + j + 8] + p22[dl2 + j + 8]) / 9;
				if (img10 <= img20 + 10 &&
					img10 >= img20 - 10 &&
					img11 <= img21 + 10 &&
					img11 >= img21 - 10 &&
					img12 <= img22 + 10 &&
					img12 >= img22 - 10)
				{
					count++;
				}

			}

		}
		if (maxcount < count)
		{
			maxcount = count;
			*dh = k;
		}
	}
	cout << "1: " << clock() - start_time << endl;
	for (int l = -50; l < 50; l++)
	{
		count = 0;
		for (int i = 0; i < row - 3; i += 3)
		{
			if ((*dh) + i + 3 >= row)
			{
				break;
			}
			p10 = img1.ptr<uchar>((*dh) + i);
			p11 = img1.ptr<uchar>((*dh) + i + 1);
			p12 = img1.ptr<uchar>((*dh) + i + 2);
			p20 = img2.ptr<uchar>(i);
			p21 = img2.ptr<uchar>(i + 1);
			p22 = img2.ptr<uchar>(i + 2);

			for (int j = 0; j < col * chanels; j += 3 * chanels)
			{

				if (abs(l) * chanels + j + 3 >= col * chanels)
				{
					break;
				}
				int dl1 = 0;
				int dl2 = 0;
				if (l < 0)
					dl1 = abs(l) * chanels;
				if (l > 0)
					dl2 = abs(l) * chanels;
				int img10 = (p10[dl1 + j] + p11[dl1 + j] + p12[dl1 + j] +
					p10[dl1 + j + 3] + p11[dl1 + j + 3] + p12[dl1 + j + 3] +
					p10[dl1 + j + 6] + p11[dl1 + j + 6] + p12[dl1 + j + 6]) / 9;
				int img11 = (p10[dl1 + j + 1] + p11[dl1 + j + 1] + p12[dl1 + j + 1] +
					p10[dl1 + j + 4] + p11[dl1 + j + 4] + p12[dl1 + j + 4] +
					p10[dl1 + j + 7] + p11[dl1 + j + 7] + p12[dl1 + j + 7]) / 9;
				int img12 = (p10[dl1 + j + 2] + p11[dl1 + j + 2] + p12[dl1 + j + 2] +
					p10[dl1 + j + 5] + p11[dl1 + j + 5] + p12[dl1 + j + 5] +
					p10[dl1 + j + 8] + p11[dl1 + j + 8] + p12[dl1 + j + 8]) / 9;
				int img20 = (p20[dl2 + j] + p21[dl2 + j] + p22[dl2 + j] +
					p20[dl2 + j + 3] + p21[dl2 + j + 3] + p22[dl2 + j + 3] +
					p20[dl2 + j + 6] + p21[dl2 + j + 6] + p22[dl2 + j + 6]) / 9;
				int img21 = (p20[dl2 + j + 1] + p21[dl2 + j + 1] + p22[dl2 + j + 1] +
					p20[dl2 + j + 4] + p21[dl2 + j + 4] + p22[dl2 + j + 4] +
					p20[dl2 + j + 7] + p21[dl2 + j + 7] + p22[dl2 + j + 7]) / 9;
				int img22 = (p20[dl2 + j + 2] + p21[dl2 + j + 2] + p22[dl2 + j + 2] +
					p20[dl2 + j + 5] + p21[dl2 + j + 5] + p22[dl2 + j + 5] +
					p20[dl2 + j + 8] + p21[dl2 + j + 8] + p22[dl2 + j + 8]) / 9;

				if (img10 <= img20 + 10 &&
					img10 >= img20 - 10 &&
					img11 <= img21 + 10 &&
					img11 >= img21 - 10 &&
					img12 <= img22 + 10 &&
					img12 >= img22 - 10)
				{
					count++;
				}

			}

		}
		if (maxcount < count)
		{
			maxcount = count;
			*dw = l;
		}
	}
	finish_time = clock();
	cout << "2: " << finish_time - start_time << endl;
	pozition_result << name_img1 << "    " << name_img2 << "    dh: " << (*dh) << "    dw: " << (*dw) << "    time: " << finish_time - start_time << endl;
}

void stitch(string* arr, int** arrdd, int S, int max_h, int num)
{
	int up_height;
	int up_width;
	Mat elem = imread(arr[S-1]);
	up_height = elem.rows;
	up_width = max_h;
	int start = 0;
	for (int i = 0; i < S-1; i++)
	{
		up_height += arrdd[i][0];
		up_width += abs(arrdd[i][1]);
		if (arrdd[i][1] < 0)
			start += abs(arrdd[i][1]);
	}
	Mat buf = imread(arr[0]);
	Mat pano = cv::Mat::Mat(up_height, up_width, buf.type(), (0, 0, 0));
	int cur = 0;
	uchar* p1;
	uchar* p2;
	int chanels = buf.channels();
	for (int i = 0; i < arrdd[0][0] + arrdd[1][0]; i++)
	{
		p1 = pano.ptr<uchar>(i);
		p2 = buf.ptr<uchar>(i);
		for (int j = 0; j < buf.cols*chanels; j++)
		{
			p1[j + start*chanels] = p2[j];
		}
	}
	start += arrdd[0][1];
	cur += arrdd[0][0];
	for (int k = 0; k < S - 2; k++)
	{
		buf = imread(arr[k+1]);
		for (int i = 0; i < 2*arrdd[k+1][0]; i++)
		{
			p1 = pano.ptr<uchar>(i + cur);
			p2 = buf.ptr<uchar>(i);
			for (int j = 0; j < buf.cols*chanels; j+=3)
			{
				if (!(p2[j] <= 30 && p2[j + 1] <= 30 && p2[j + 2] <= 30))
				{
					p1[start * chanels + j] = p2[j];
					p1[start * chanels + j + 1] = p2[j + 1];
					p1[start * chanels + j + 2] = p2[j + 2];
				}
			}
		}
		start += arrdd[k + 1][1];
		cur += arrdd[k+1][0];
	}
	buf = imread(arr[S - 1]);
	for (int i = 0;i< buf.rows; i++)
	{
		p1 = pano.ptr<uchar>(i + cur);
		p2 = buf.ptr<uchar>(i);
		for (int j = 0; j < buf.cols*chanels; j+=3)
		{
			if (i + cur >= pano.rows || j >= pano.cols*chanels)
			{
				cout << "jopa " << i + cur << " - " << pano.rows << ", " << j + start << " - " << pano.cols << endl;
			}
			if (!(p2[j] <= 30 && p2[j + 1] <= 30 && p2[j + 2] <= 30))
			{
				p1[start * chanels + j] = p2[j];
				p1[start * chanels + j + 1] = p2[j + 1];
				p1[start * chanels + j + 2] = p2[j + 2];
			}
		}
	}
	imwrite("answer" + to_string(num) + ".jpg", pano);

}

int main()
{
	string arr1[] = {"2020_07_03_PhotoCamera_g401b40179_f001_005.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_006.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_007.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_008.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_009.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_010.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_011.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_012.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_013.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_014.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_015.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_016.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_017.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_018.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_019.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_020.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_021.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_022.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_023.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_024.JPG",
	};
	int S1 = 20;
	
	string arr2[] = {
		"2020_07_03_PhotoCamera_g401b40179_f001_040.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_039.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_038.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_037.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_036.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_035.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_034.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_033.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_032.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_031.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_030.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_029.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_028.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_027.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_026.JPG",
		"2020_07_03_PhotoCamera_g401b40179_f001_025.JPG"
	};
	int S2 = 16;
	ofstream pozition_result("pozition_result.txt");
	Mat img1 = imread(arr1[0]);
	imwrite("buffer.jpg", img1);
	int** arrdd1 = new int*[S1];
	for (int i = 0; i < S1; i++)
		arrdd1[i] = new int[2];
	int dh1;
	int dw1;
	int max_w1 = 0;
	for (int i = 0; i < S1 - 1; i++)
	{
		cout << arr1[i] << "     " << arr1[i + 1] << endl;
		unsigned int start_time = clock();
		counting(arr1[i], arr1[i+1], & dh1, &dw1, &max_w1, pozition_result);
		unsigned int end_time = clock(); 
		cout << "time: " << end_time - start_time << endl;
		arrdd1[i][0] = dh1;
		arrdd1[i][1] = dw1;
	}
	unsigned int start_time = clock();
	stitch(arr1, arrdd1, S1, max_w1, 1);
	unsigned int end_time = clock();
	pozition_result << "time stitch: " << end_time - start_time << endl;
	Mat img2 = imread(arr2[0]);
	imwrite("buffer.jpg", img2);
	int** arrdd2 = new int* [S2];
	for (int i = 0; i < S2; i++)
		arrdd2[i] = new int[2];
	int dh2;
	int dw2;
	int max_w2 = 0;
	for (int i = 0; i < S2 - 1; i++)
	{
		cout << arr2[i] << "     " << arr2[i + 1] << endl;
		unsigned int start_time = clock();
		counting(arr2[i], arr2[i + 1], &dh2, &dw2, &max_w2, pozition_result);
		unsigned int end_time = clock();
		cout << "time: " << end_time - start_time << endl;
		arrdd2[i][0] = dh2;
		arrdd2[i][1] = dw2;
	}
	start_time = clock();
	stitch(arr2, arrdd2, S2, max_w2, 2);
	end_time = clock();
	pozition_result << "time stitch: " << end_time - start_time << endl;

	return 0;
}