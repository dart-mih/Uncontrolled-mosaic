// Программа CPP для стежка
// ввод изображений (панорамы) с использованием OpenCV
#include <iostream>
#include <fstream>
#include <stdlib.h>

// Включить заголовочные файлы из каталога OpenCV
// требуется для сшивания изображений.
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"
#include <ctime>


using namespace std;

using namespace cv;

void counting(string name_img1, string name_img2, int* dh, int* dw, int* max_h)
{
	Mat img1 = imread(name_img1);
	Mat img2 = imread(name_img2);
	if (img1.cols > (*max_h))
		(*max_h) = img1.cols;
	if (img2.cols > (*max_h))
		(*max_h) = img2.cols;
// Отобразить на изображении белую пунктирную сетку
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
	for (int l = -20; l < 20; l++)
	{
		for (int k = row / 4; k < row / 2; k++)
		{
			//cout << "K " << k << endl;
			count = 0;
			int check = 0;
			for (int i = 0; i < img1.rows; i += 3)
			{
				if (check == 1)
					break;
				for (int j = 0; j < col; j += 3)
				{
					if (k + i + 3 >= img1.rows || abs(l) + j + 3 >= col)
					{
						check = 1;
						break;
					}
					int dl1 = 0;
					int dl2 = 0;
					if (l < 0)
						dl1 = abs(l);
					if (l > 0)
						dl2 = abs(l);
					//cout << dl1 << ' ' << dl2 << endl;
					int img10 = (img1.at<Vec3b>(i + k, dl1 + j)[0] + img1.at<Vec3b>(i + k + 1, dl1 + j)[0] + img1.at<Vec3b>(i + k + 2, dl1 + j)[0] +
						img1.at<Vec3b>(i + k, dl1 + j + 1)[0] + img1.at<Vec3b>(i + k + 1, dl1 + j + 1)[0] + img1.at<Vec3b>(i + k + 2, dl1 + j + 1)[0] +
						img1.at<Vec3b>(i + k, dl1 + j + 2)[0] + img1.at<Vec3b>(i + k + 1, dl1 + j + 2)[0] + img1.at<Vec3b>(i + k + 2, dl1 + j + 2)[0]) / 9;
					int img11 = (img1.at<Vec3b>(i + k, dl1 + j)[1] + img1.at<Vec3b>(i + k + 1, dl1 + j)[1] + img1.at<Vec3b>(i + k + 2, dl1 + j)[1] +
						img1.at<Vec3b>(i + k, j + dl1 + 1)[1] + img1.at<Vec3b>(i + k + 1, dl1 + j + 1)[1] + img1.at<Vec3b>(i + k + 2, dl1 + j + 1)[1] +
						img1.at<Vec3b>(i + k, j + dl1 + 2)[1] + img1.at<Vec3b>(i + k + 1, dl1 + j + 2)[1] + img1.at<Vec3b>(i + k + 2, dl1 + j + 2)[1]) / 9;
					int img12 = (img1.at<Vec3b>(i + k, dl1 + j)[2] + img1.at<Vec3b>(i + k + 1, dl1 + j)[2] + img1.at<Vec3b>(i + k + 2, dl1 + j)[2] +
						img1.at<Vec3b>(i + k, dl1 + j + 1)[2] + img1.at<Vec3b>(i + k + 1, dl1 + j + 1)[2] + img1.at<Vec3b>(i + k + 2, dl1 + j + 1)[2] +
						img1.at<Vec3b>(i + k, dl1 + j + 2)[2] + img1.at<Vec3b>(i + k + 1, dl1 + j + 2)[2] + img1.at<Vec3b>(i + k + 2, dl1 + j + 2)[2]) / 9;
					int img20 = (img2.at<Vec3b>(i, dl2 + j)[0] + img2.at<Vec3b>(i + 1, dl2 + j)[0] + img2.at<Vec3b>(i + 2, dl2 + j)[0] +
						img2.at<Vec3b>(i, dl2 + j + 1)[0] + img2.at<Vec3b>(i + 1, dl2 + j + 1)[0] + img2.at<Vec3b>(i + 2, dl2 + j + 1)[0] +
						img2.at<Vec3b>(i, dl2 + j + 2)[0] + img2.at<Vec3b>(i + 1, dl2 + j + 2)[0] + img2.at<Vec3b>(i + 2, dl2 + j + 2)[0]) / 9;
					int img21 = (img2.at<Vec3b>(i + k, dl2 + j)[1] + img2.at<Vec3b>(i + 1, dl2 + j)[1] + img2.at<Vec3b>(i + 2, dl2 + j)[1] +
						img2.at<Vec3b>(i, dl2 + j + 1)[1] + img2.at<Vec3b>(i + 1, dl2 + j + 1)[1] + img2.at<Vec3b>(i + 2, dl2 + j + 1)[1] +
						img2.at<Vec3b>(i, dl2 + j + 2)[1] + img2.at<Vec3b>(i + 1, dl2 + j + 2)[1] + img2.at<Vec3b>(i + 2, dl2 + j + 2)[1]) / 9;
					int img22 = (img2.at<Vec3b>(i + k, dl2 + j)[2] + img2.at<Vec3b>(i + 1, dl2 + j)[2] + img2.at<Vec3b>(i + 2, dl2 + j)[2] +
						img2.at<Vec3b>(i, dl2 + j + 1)[2] + img2.at<Vec3b>(i + 1, dl2 + j + 1)[2] + img2.at<Vec3b>(i + 2, dl2 + j + 1)[2] +
						img2.at<Vec3b>(i, dl2 + j + 2)[2] + img2.at<Vec3b>(i + 1, dl2 + j + 2)[2] + img2.at<Vec3b>(i + 2, dl2 + j + 2)[2]) / 9;
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
				//cout <<"count: "<< count << endl;
				maxcount = count;
				*dh = k;
				*dw = l;
			}
		}
	}

	/*
	int up_width = img1.cols + abs(*dw);
	int up_height = img1.rows;
	cout << "k:" << *dh << endl << "l:" << *dw << endl;
	Mat ans = cv::Mat::Mat(up_height, up_width,	img1.type(), (0, 0, 0));
	//resize(img1, ans, Size(up_width, up_height), INTER_LINEAR);
	int dw1 = 0;
	int dw2 = 0;
	if (*dw > 0)
		dw2 = (*dw);
	else
		dw1 = abs(*dw);
	cout << dw1 << ' ' << dw2 << endl;
	for (int i = 2 * img1.rows - img2.rows - (*dh) - 100; i < up_height; i++)
	{
		cout << i << endl;
		for (int j = 0; j < img2.cols; j++)
		{
			cout << dw2 << ' ' << j << ' ' << i + img2.rows - img1.rows - 2 * (*dh) << endl;
			ans.at<Vec3b>(i, dw2 + j)[0] = img2.at<Vec3b>(i + img2.rows - img1.rows - 2 * (*dh), j)[0];
			ans.at<Vec3b>(i, dw2 + j)[1] = img2.at<Vec3b>(i + img2.rows - img1.rows - 2 * (*dh), j)[1];
			ans.at<Vec3b>(i, dw2 + j)[2] = img2.at<Vec3b>(i + img2.rows - img1.rows - 2 * (*dh), j)[2];
		}
	}
	
	for (int i = 0; i < 2*img1.rows - img2.rows - (*dh); i++)
	{
		//cout << img2.rows - img1.rows + (*dh) + i << endl;
		for (int j = 0; j < img1.cols; j++)
		{
			ans.at<Vec3b>(i, dw1 + j)[0] = img1.at<Vec3b>(img2.rows - img1.rows + (*dh) + i, j)[0];
			ans.at<Vec3b>(i, dw1 + j)[1] = img1.at<Vec3b>(img2.rows - img1.rows + (*dh) + i, j)[1];
			ans.at<Vec3b>(i, dw1 + j)[2] = img1.at<Vec3b>(img2.rows - img1.rows + (*dh) + i, j)[2];
		}
	}
	cout << 2 * img1.rows - img2.rows + (*dh) - 100 << endl;*/
	
	
	//imwrite("buffer.jpg", ans);
}

void stitch(string* arr, int** arrdd, int S, int max_h)
{
	int up_height;
	int up_width;
	Mat elem = imread(arr[S-1]);
	up_height = elem.rows;
	up_width = max_h;
	//cout << "___________" << endl;
	int start = 0;
	for (int i = 0; i < S-1; i++)
	{
		//cout << arrdd[i][0] << ' ' << arrdd[i][1] << endl;
		up_height += arrdd[i][0];
		up_width += abs(arrdd[i][1]);
		if (arrdd[i][1] < 0)
			start += abs(arrdd[i][1]);
	}
	Mat buf = imread(arr[0]);
	Mat pano = cv::Mat::Mat(up_height, up_width, buf.type(), (0, 0, 0));
	int cur = 0;
	//cout << "begin " << up_height << ' ' << up_width << endl;
	
	for (int i = 0; i < arrdd[0][0]; i++)
	{
		for (int j = 0; j < buf.cols; j++)
		{
			pano.at<Vec3b>(i + start, j)[0] = buf.at<Vec3b>(i, j)[0];
			pano.at<Vec3b>(i + start, j)[1] = buf.at<Vec3b>(i, j)[1];
			pano.at<Vec3b>(i + start, j)[2] = buf.at<Vec3b>(i, j)[2];
		}
	}
	start += arrdd[0][1];
	cur += arrdd[0][0];
	for (int k = 0; k < S - 2; k++)
	{
		buf = imread(arr[k+1]);
;		//cout << k <<' '<<cur << endl;
		//cout << arr[k + 1] << ' ' << buf.rows << ' ' << buf.cols << ' ' << start << ' ' << cur << endl;
		for (int i = 0; i < arrdd[k+1][0]; i++)
		{
			//cout << "i: " << i << endl;
			//cout << i + cur << ' ' << dw1 << ' ' << i << ' ' << dw2 << endl;
			
			//cout << buf.at<Vec3b>(0, 0)[0] << endl;
			for (int j = 0; j < buf.cols; j++)
			{
				/*pano.at<Vec3b>(i + cur, start + j)[0] = 255;
				pano.at<Vec3b>(i + cur, start + j)[1] = 255;
				pano.at<Vec3b>(i + cur, start + j)[2] = 255;*/
				pano.at<Vec3b>(i + cur, start + j)[0] = buf.at<Vec3b>(i, j)[0];
				pano.at<Vec3b>(i + cur, start + j)[1] = buf.at<Vec3b>(i, j)[1];
				pano.at<Vec3b>(i + cur, start + j)[2] = buf.at<Vec3b>(i, j)[2];
			}
		}
		start += arrdd[k + 1][1];
		cur += arrdd[k+1][0];
//		elem = imread(arr[k + 1]);
	}
	buf = imread(arr[S - 1]);
	//cout << "complete" << endl;
	for (int i = 0;i< buf.rows; i++)
	{
		for (int j = 0; j < buf.cols; j++)
		{
			if (i + cur >= pano.rows || j + start >= pano.cols)
			{
				cout << "jopa " << i + cur << " - " << pano.rows << ", " << j + start << " - " << pano.cols << endl;
			}
			//cout << i + cur << ' ' << dw1 + j << ' ' << i << ' ' << dw2 + j << endl;
			/*pano.at<Vec3b>(i + cur, start + j)[0] = 255;
			pano.at<Vec3b>(i + cur, start + j)[1] = 255;
			pano.at<Vec3b>(i + cur, start + j)[2] = 255;*/
			
			pano.at<Vec3b>(i + cur, start + j)[0] = buf.at<Vec3b>(i, j)[0];
			pano.at<Vec3b>(i + cur, start + j)[1] = buf.at<Vec3b>(i, j)[1];
			pano.at<Vec3b>(i + cur, start + j)[2] = buf.at<Vec3b>(i, j)[2];
		}
	}
	imwrite("answer2.jpg", pano);
}


int main()
{
	//         ...
	
	//unsigned int search_time = end_time - start_time; // искомое время
	/*string arr[] = {"2020_07_03_PhotoCamera_g401b40179_f001_005.JPG",
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
	int S = 20;*/
	
	string arr[] = {
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
	int S = 16;
	
	
	//cout << arr[0] << endl;
	Mat img = imread(arr[0]);
	imwrite("buffer.jpg", img);
	int** arrdd = new int*[S];
	for (int i = 0; i < S; i++)
		arrdd[i] = new int[2];
	int dh;
	int dw;
	//cout << S << endl;
	int max_w = 0;
	for (int i = 0; i < S - 1; i++)
	{
		cout << arr[i] << "     " << arr[i + 1] << endl;
		unsigned int start_time = clock();
		counting(arr[i], arr[i+1], & dh, &dw, &max_w);
		unsigned int end_time = clock(); // конечное время
		cout << "time: " << end_time - start_time << endl;
		arrdd[i][0] = dh;
		arrdd[i][1] = dw;
		//cout << i<<' ' << arr[i] << ' ' << dh << ' ' << dw << endl;
	}
	unsigned int start_time = clock();
	stitch(arr, arrdd, S, max_w);
	unsigned int end_time = clock(); // конечное время
	cout << "time stitch: " << end_time - start_time << endl;
	return 0;
}