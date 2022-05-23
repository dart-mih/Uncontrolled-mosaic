#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string> 

// Include header files from OpenCV directory
// required for image stitching
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include "../Shots_normalization/PhotoAndCameraInf.h"

using namespace std;
using namespace cv;

/*
Algorithm for matching images by pixel comparision with some optimization.
first_img - image relative to which the position of the second is searched.
second_img - frame following the first image.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
horizontal_shift - в каком промежутке относительно центра искать смещение по Ox.
*/
Point pixelCompareAlg(Mat& first_img, Mat& second_img, PhotoInf& first_photo_inf, PhotoInf& second_photo_inf, int horizontal_shift=50) {
	int count = 0;
	int maxcount = 0;

	Point relative_pos = Point(0, -100);

	int row;
	int col;

	// Comparison limits.
	if (first_img.rows < second_img.rows)
		row = first_img.rows;
	else
		row = second_img.rows;

	if (first_img.cols < second_img.cols)
		col = first_img.cols;
	else
		col = second_img.cols;

	// Optimization. Choice of area of interest y.
	int y_start_limit = row / 4;
	int y_end_limit = (2 * row) / 5;

	if (second_photo_inf.latitude - first_photo_inf.latitude > 0) {
		y_start_limit = -(2 * row) / 5;
		y_end_limit = -row / 4;
	}

	int chanels = first_img.channels();

	uchar* p10;
	uchar* p11;
	uchar* p12;
	uchar* p20;
	uchar* p21;
	uchar* p22;

	// The next step is to compare the image by the median of 9 pixels.
	// k - responsible for shifting the 2nd image vertically
	// j and i - columns and rows of compared images.
	for (int k = y_start_limit; k < y_end_limit; k += 3) {
		count = 0;
		for (int i = 0; i < row - 3; i += 3) {
			if ((k + i < 0) || (k + i + 3 >= row)) {
				continue;
			}
			p10 = first_img.ptr<uchar>(k + i);
			p11 = first_img.ptr<uchar>(k + i + 1);
			p12 = first_img.ptr<uchar>(k + i + 2);
			p20 = second_img.ptr<uchar>(i);
			p21 = second_img.ptr<uchar>(i + 1);
			p22 = second_img.ptr<uchar>(i + 2);

			for (int j = 0; j < col * chanels; j += 3 * chanels) {
				if (j + 3 >= col * chanels) {
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
					img12 >= img22 - 10) {
					count++;
				}
			}
		}
		if (maxcount < count) {
			maxcount = count;
			relative_pos.y = k;
		}
	}
	// Here, the best horizontal match is sought with the new found height.
	// l - horizontal shift.
	for (int l = -horizontal_shift; l < horizontal_shift; l++) {
		count = 0;
		for (int i = 0; i < row - 3; i += 3) {
			if ((relative_pos.y + i < 0) || ((relative_pos.y) + i + 3) >= row) {
				continue;
			}
			p10 = first_img.ptr<uchar>(relative_pos.y + i);
			p11 = first_img.ptr<uchar>(relative_pos.y + i + 1);
			p12 = first_img.ptr<uchar>(relative_pos.y + i + 2);
			p20 = second_img.ptr<uchar>(i);
			p21 = second_img.ptr<uchar>(i + 1);
			p22 = second_img.ptr<uchar>(i + 2);

			for (int j = 0; j < col * chanels; j += 3 * chanels) {

				if (abs(l) * chanels + j + 3 >= col * chanels) {
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
					img12 >= img22 - 10) {
					count++;
				}
			}
		}
		if (maxcount < count) {
			maxcount = count;
			relative_pos.x = l;
		}
	}
	return relative_pos;
}