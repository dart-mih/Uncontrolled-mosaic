#pragma once

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
mask_first - mask, values 255 in which correspond to pixels that will not be taken into account when working with the first image.
mask_second - mask, values 255 in which correspond to pixels that will not be taken into account when working with the second picture.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
vertical_shift - in which interval relative to the center_search_pos to look for the offset along Oy.
horizontal_shift - in which interval relative to the center_search_pos to look for the offset along Ox.
center_search_pos - center of the area in which we are looking for the offset of the second image left top corner relative to the first image corner.
*/
Point pixelCompareAlg(Mat& first_img, Mat& second_img, Mat& mask_first, Mat& mask_second, PhotoInf& first_photo_inf,
	PhotoInf& second_photo_inf, int vertical_shift, int horizontal_shift = 50, Point center_search_pos = Point{0, 0}) {
	long int count = 0;
	long int count_all = 0;
	double maxcount = 0;
	
	Point relative_pos = Point(0, -100);

	// Compute limits.
	int y_start_limit = center_search_pos.y - vertical_shift;
	int y_end_limit = center_search_pos.y + vertical_shift;

	int x_start_limit = center_search_pos.x - horizontal_shift;
	int x_end_limit = center_search_pos.x + horizontal_shift;

	int chanels = first_img.channels();

	uchar* p10;
	uchar* p11;
	uchar* p12;
	uchar* p20;
	uchar* p21;
	uchar* p22;

	uchar* mask_p10;
	uchar* mask_p11;
	uchar* mask_p12;
	uchar* mask_p20;
	uchar* mask_p21;
	uchar* mask_p22;

	// The next step is to compare the image by the median of 9 pixels.
	// k - responsible for shifting the 2nd image vertically
	// j and i - columns and rows of compared images.
	int j_start = 0;
	if (center_search_pos.x < 0) {
		j_start = -center_search_pos.x * chanels;
	}
	for (int k = y_start_limit; k < y_end_limit; k += 3) {
		count = 0;
		count_all = 0;
		for (int i = 0; i < first_img.rows - 3; i += 3) {
			if ((k + i < 0) || (k + i + 3 >= first_img.rows) || (i + 3 >= second_img.rows)) {
				continue;
			}
			p10 = first_img.ptr<uchar>(k + i);
			p11 = first_img.ptr<uchar>(k + i + 1);
			p12 = first_img.ptr<uchar>(k + i + 2);
			p20 = second_img.ptr<uchar>(i);
			p21 = second_img.ptr<uchar>(i + 1);
			p22 = second_img.ptr<uchar>(i + 2);

			// Take the same rows from the mask.
			mask_p10 = mask_first.ptr<uchar>(k + i);
			mask_p11 = mask_first.ptr<uchar>(k + i + 1);
			mask_p12 = mask_first.ptr<uchar>(k + i + 2);
			mask_p20 = mask_second.ptr<uchar>(i);
			mask_p21 = mask_second.ptr<uchar>(i + 1);
			mask_p22 = mask_second.ptr<uchar>(i + 2);

			for (int j = j_start; j < first_img.cols * chanels; j += 3*chanels) {
				if ((center_search_pos.x*chanels) + j + (3*chanels) >= first_img.cols *chanels) {
					break;
				}
				int dl1 = center_search_pos.x * chanels;
				int img10 = (p10[dl1 + j] + p11[dl1 + j] + p12[dl1 + j] +
							 p10[dl1 + j + 3] + p11[dl1 + j + 3] + p12[dl1 + j + 3] +
							 p10[dl1 + j + 6] + p11[dl1 + j + 6] + p12[dl1 + j + 6]) / 9;
				int img11 = (p10[dl1 + j + 1] + p11[dl1 + j + 1] + p12[dl1 + j + 1] +
							 p10[dl1 + j + 4] + p11[dl1 + j + 4] + p12[dl1 + j + 4] +
							 p10[dl1 + j + 7] + p11[dl1 + j + 7] + p12[dl1 + j + 7]) / 9;
				int img12 = (p10[dl1 + j + 2] + p11[dl1 + j + 2] + p12[dl1 + j + 2] +
							 p10[dl1 + j + 5] + p11[dl1 + j + 5] + p12[dl1 + j + 5] +
							 p10[dl1 + j + 8] + p11[dl1 + j + 8] + p12[dl1 + j + 8]) / 9;
				int img20 = (p20[j] + p21[j] + p22[j] +
							 p20[j + 3] + p21[j + 3] + p22[j + 3] +
							 p20[j + 6] + p21[j + 6] + p22[j + 6]) / 9;
				int img21 = (p20[j + 1] + p21[j + 1] + p22[j + 1] +
							 p20[j + 4] + p21[j + 4] + p22[j + 4] +
							 p20[j + 7] + p21[j + 7] + p22[j + 7]) / 9;
				int img22 = (p20[j + 2] + p21[j + 2] + p22[j + 2] +
							 p20[j + 5] + p21[j + 5] + p22[j + 5] +
							 p20[j + 8] + p21[j + 8] + p22[j + 8]) / 9;

				if (mask_p10[(dl1 + j) / chanels] > 0 && mask_p10[(dl1 + j) / chanels + 1] > 0 && mask_p10[(dl1 + j) / chanels + 2] > 0 &&
					mask_p11[(dl1 + j) / chanels] > 0 && mask_p11[(dl1 + j) / chanels + 1] > 0 && mask_p11[(dl1 + j) / chanels + 2] > 0 &&
					mask_p12[(dl1 + j) / chanels] > 0 && mask_p12[(dl1 + j) / chanels + 1] > 0 && mask_p12[(dl1 + j) / chanels + 2] > 0 &&
					mask_p20[j / chanels] > 0 && mask_p20[j / chanels + 1] > 0 && mask_p20[j / chanels + 2] > 0 &&
					mask_p21[j / chanels] > 0 && mask_p21[j / chanels + 1] > 0 && mask_p21[j / chanels + 2] > 0 &&
					mask_p22[j / chanels] > 0 && mask_p22[j / chanels + 1] > 0 && mask_p22[j / chanels + 2] > 0) {
					if (img10 <= img20 + 10 &&
						img10 >= img20 - 10 &&
						img11 <= img21 + 10 &&
						img11 >= img21 - 10 &&
						img12 <= img22 + 10 &&
						img12 >= img22 - 10) {
						count++;
					}
					count_all++;
				}
			}
		}
		if (maxcount < (double)count / count_all) {
			maxcount = (double)count / count_all;
			relative_pos.y = k;
		}
	}
	// Here, the best horizontal match is sought with the new found height.
	// l - horizontal shift.
	int i_start = 0;
	if (relative_pos.y < 0) {
		i_start = -relative_pos.y;
	}
	for (int l = x_start_limit; l < x_end_limit; l += 3) {
		count = 0;
		count_all = 0;
		for (int i = i_start; i < first_img.rows - 3; i += 3) {
			if ((relative_pos.y + i + 3 >= first_img.rows)  || (i + 3 >= second_img.rows)) {
				break;
			}
			p10 = first_img.ptr<uchar>(relative_pos.y + i);
			p11 = first_img.ptr<uchar>(relative_pos.y + i + 1);
			p12 = first_img.ptr<uchar>(relative_pos.y + i + 2);
			p20 = second_img.ptr<uchar>(i);
			p21 = second_img.ptr<uchar>(i + 1);
			p22 = second_img.ptr<uchar>(i + 2);

			// Take the same rows from the mask.
			mask_p10 = mask_first.ptr<uchar>(relative_pos.y + i);
			mask_p11 = mask_first.ptr<uchar>(relative_pos.y + i + 1);
			mask_p12 = mask_first.ptr<uchar>(relative_pos.y + i + 2);
			mask_p20 = mask_second.ptr<uchar>(i);
			mask_p21 = mask_second.ptr<uchar>(i + 1);
			mask_p22 = mask_second.ptr<uchar>(i + 2);

			for (int j = 0; j < first_img.cols * chanels; j += 3 * chanels) {
				if (((l * chanels) + j < 0) || ((l* chanels) + j + (3 * chanels) > first_img.cols * chanels) ||
					(j + (3 * chanels) > second_img.cols * chanels)) {
					continue;
				}
				int dl1 = l * chanels;
				int img10 = (p10[dl1 + j] + p11[dl1 + j] + p12[dl1 + j] +
							 p10[dl1 + j + 3] + p11[dl1 + j + 3] + p12[dl1 + j + 3] +
							 p10[dl1 + j + 6] + p11[dl1 + j + 6] + p12[dl1 + j + 6]) / 9;
				int img11 = (p10[dl1 + j + 1] + p11[dl1 + j + 1] + p12[dl1 + j + 1] +
							 p10[dl1 + j + 4] + p11[dl1 + j + 4] + p12[dl1 + j + 4] +
							 p10[dl1 + j + 7] + p11[dl1 + j + 7] + p12[dl1 + j + 7]) / 9;
				int img12 = (p10[dl1 + j + 2] + p11[dl1 + j + 2] + p12[dl1 + j + 2] +
							 p10[dl1 + j + 5] + p11[dl1 + j + 5] + p12[dl1 + j + 5] +
							 p10[dl1 + j + 8] + p11[dl1 + j + 8] + p12[dl1 + j + 8]) / 9;
				int img20 = (p20[j] + p21[j] + p22[j] +
							 p20[j + 3] + p21[j + 3] + p22[j + 3] +
							 p20[j + 6] + p21[j + 6] + p22[j + 6]) / 9;
				int img21 = (p20[j + 1] + p21[j + 1] + p22[j + 1] +
							 p20[j + 4] + p21[j + 4] + p22[j + 4] +
							 p20[j + 7] + p21[j + 7] + p22[j + 7]) / 9;
				int img22 = (p20[j + 2] + p21[j + 2] + p22[j + 2] +
							 p20[j + 5] + p21[j + 5] + p22[j + 5] +
					p20[j + 8] + p21[j + 8] + p22[j + 8]) / 9;

				if (mask_p10[(dl1 + j) / chanels] > 0 && mask_p10[(dl1 + j) / chanels + 1] > 0 && mask_p10[(dl1 + j) / chanels + 2] > 0 &&
					mask_p11[(dl1 + j) / chanels] > 0 && mask_p11[(dl1 + j) / chanels + 1] > 0 && mask_p11[(dl1 + j) / chanels + 2] > 0 &&
					mask_p12[(dl1 + j) / chanels] > 0 && mask_p12[(dl1 + j) / chanels + 1] > 0 && mask_p12[(dl1 + j) / chanels + 2] > 0 &&
					mask_p20[j / chanels] > 0 && mask_p20[j / chanels + 1] > 0 && mask_p20[j / chanels + 2] > 0 &&
					mask_p21[j / chanels] > 0 && mask_p21[j / chanels + 1] > 0 && mask_p21[j / chanels + 2] > 0 &&
					mask_p22[j / chanels] > 0 && mask_p22[j / chanels + 1] > 0 && mask_p22[j / chanels + 2] > 0) {
					if (img10 <= img20 + 10 &&
						img10 >= img20 - 10 &&
						img11 <= img21 + 10 &&
						img11 >= img21 - 10 &&
						img12 <= img22 + 10 &&
						img12 >= img22 - 10) {
						count++;
					}
					count_all++;
				}
			}
		}
		if (maxcount < (double)count / count_all) {
			maxcount = (double)count / count_all;
			relative_pos.x = l;
		}
	}
	return relative_pos;
}

/*
Algorithm for matching images by pixel comparision for one chanel.
first_img - image relative to which the position of the second is searched.
second_img - frame following the first image.
mask_first - mask, values 255 in which correspond to pixels that will not be taken into account when working with the first image.
mask_second - mask, values 255 in which correspond to pixels that will not be taken into account when working with the second picture.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
vertical_shift - in which interval relative to the center_search_pos to look for the offset along Oy.
horizontal_shift - in which interval relative to the center_search_pos to look for the offset along Ox.
center_search_pos - center of the area in which we are looking for the offset of the second image left top corner relative to the first image corner.
*/
Point pixelCompareAlgOneChanel(Mat& first_img, Mat& second_img, Mat& mask_first, Mat& mask_second, PhotoInf& first_photo_inf,
	PhotoInf& second_photo_inf, int vertical_shift, int horizontal_shift = 50, Point center_search_pos = Point{ 0, 0 }) {
	long int count = 0;
	long int count_all = 0;
	double maxcount = 0;

	Point relative_pos = Point(0, -100);

	// Compute limits.
	int y_start_limit = center_search_pos.y - vertical_shift;
	int y_end_limit = center_search_pos.y + vertical_shift;

	int x_start_limit = center_search_pos.x - horizontal_shift;
	int x_end_limit = center_search_pos.x + horizontal_shift;

	int chanels = first_img.channels();

	uchar* p10;
	uchar* p11;
	uchar* p12;
	uchar* p20;
	uchar* p21;
	uchar* p22;

	uchar* mask_p10;
	uchar* mask_p11;
	uchar* mask_p12;
	uchar* mask_p20;
	uchar* mask_p21;
	uchar* mask_p22;

	// The next step is to compare the image by the median of 9 pixels.
	// k - responsible for shifting the 2nd image vertically
	// j and i - columns and rows of compared images.
	int j_start = 0;
	if (center_search_pos.x < 0) {
		j_start = -center_search_pos.x * chanels;
	}
	for (int k = y_start_limit; k < y_end_limit; k += 3) {
		count = 0;
		count_all = 0;
		for (int i = 0; i < first_img.rows - 3; i += 3) {
			if ((k + i < 0) || (k + i + 3 >= first_img.rows) || (i + 3 >= second_img.rows)) {
				continue;
			}
			p10 = first_img.ptr<uchar>(k + i);
			p11 = first_img.ptr<uchar>(k + i + 1);
			p12 = first_img.ptr<uchar>(k + i + 2);
			p20 = second_img.ptr<uchar>(i);
			p21 = second_img.ptr<uchar>(i + 1);
			p22 = second_img.ptr<uchar>(i + 2);

			// Take the same rows from the mask.
			mask_p10 = mask_first.ptr<uchar>(k + i);
			mask_p11 = mask_first.ptr<uchar>(k + i + 1);
			mask_p12 = mask_first.ptr<uchar>(k + i + 2);
			mask_p20 = mask_second.ptr<uchar>(i);
			mask_p21 = mask_second.ptr<uchar>(i + 1);
			mask_p22 = mask_second.ptr<uchar>(i + 2);

			for (int j = j_start; j < first_img.cols * chanels; j += 3 * chanels) {
				if ((center_search_pos.x * chanels) + j + (3 * chanels) >= first_img.cols * chanels) {
					break;
				}
				int dl1 = center_search_pos.x * chanels;

				int img1 = (p10[dl1 + j] + p11[dl1 + j] + p12[dl1 + j] +
					p10[dl1 + j + 3] + p11[dl1 + j + 3] + p12[dl1 + j + 3] +
					p10[dl1 + j + 6] + p11[dl1 + j + 6] + p12[dl1 + j + 6]) / 9;
				int img2 = (p20[j] + p21[j] + p22[j] +
					p20[j + 3] + p21[j + 3] + p22[j + 3] +
					p20[j + 6] + p21[j + 6] + p22[j + 6]) / 9;

				if (mask_p10[(dl1 + j) / chanels] == 0 && mask_p10[(dl1 + j) / chanels + 1] == 0 && mask_p10[(dl1 + j) / chanels + 2] == 0 &&
					mask_p11[(dl1 + j) / chanels] == 0 && mask_p11[(dl1 + j) / chanels + 1] == 0 && mask_p11[(dl1 + j) / chanels + 2] == 0 &&
					mask_p12[(dl1 + j) / chanels] == 0 && mask_p12[(dl1 + j) / chanels + 1] == 0 && mask_p12[(dl1 + j) / chanels + 2] == 0 &&
					mask_p20[j / chanels] == 0 && mask_p20[j / chanels + 1] == 0 && mask_p20[j / chanels + 2] == 0 &&
					mask_p21[j / chanels] == 0 && mask_p21[j / chanels + 1] == 0 && mask_p21[j / chanels + 2] == 0 &&
					mask_p22[j / chanels] == 0 && mask_p22[j / chanels + 1] == 0 && mask_p22[j / chanels + 2] == 0) {
					if (img1 <= img2 + 10 &&
						img1 >= img2 - 10) {
						count++;
					}
					count_all++;
				}
			}
		}
		if (maxcount < (double)count / count_all) {
			maxcount = (double)count / count_all;
			relative_pos.y = k;
		}
	}
	// Here, the best horizontal match is sought with the new found height.
	// l - horizontal shift.
	int i_start = 0;
	if (relative_pos.y < 0) {
		i_start = -relative_pos.y;
	}
	for (int l = x_start_limit; l < x_end_limit; l += 3) {
		count = 0;
		count_all = 0;
		for (int i = i_start; i < first_img.rows - 3; i += 3) {
			if ((relative_pos.y + i + 3 >= first_img.rows) || (i + 3 >= second_img.rows)) {
				break;
			}
			p10 = first_img.ptr<uchar>(relative_pos.y + i);
			p11 = first_img.ptr<uchar>(relative_pos.y + i + 1);
			p12 = first_img.ptr<uchar>(relative_pos.y + i + 2);
			p20 = second_img.ptr<uchar>(i);
			p21 = second_img.ptr<uchar>(i + 1);
			p22 = second_img.ptr<uchar>(i + 2);

			// Take the same rows from the mask.
			mask_p10 = mask_first.ptr<uchar>(relative_pos.y + i);
			mask_p11 = mask_first.ptr<uchar>(relative_pos.y + i + 1);
			mask_p12 = mask_first.ptr<uchar>(relative_pos.y + i + 2);
			mask_p20 = mask_second.ptr<uchar>(i);
			mask_p21 = mask_second.ptr<uchar>(i + 1);
			mask_p22 = mask_second.ptr<uchar>(i + 2);

			for (int j = 0; j < first_img.cols * chanels; j += 3 * chanels) {
				if (((l * chanels) + j < 0) || ((l * chanels) + j + (3 * chanels) > first_img.cols * chanels) ||
					(j + (3 * chanels) > second_img.cols * chanels)) {
					continue;
				}
				int dl1 = l * chanels;

				int img1 = (p10[dl1 + j] + p11[dl1 + j] + p12[dl1 + j] +
					p10[dl1 + j + 3] + p11[dl1 + j + 3] + p12[dl1 + j + 3] +
					p10[dl1 + j + 6] + p11[dl1 + j + 6] + p12[dl1 + j + 6]) / 9;
				int img2 = (p20[j] + p21[j] + p22[j] +
					p20[j + 3] + p21[j + 3] + p22[j + 3] +
					p20[j + 6] + p21[j + 6] + p22[j + 6]) / 9;

				if (mask_p10[(dl1 + j) / chanels] == 0 && mask_p10[(dl1 + j) / chanels + 1] == 0 && mask_p10[(dl1 + j) / chanels + 2] == 0 &&
					mask_p11[(dl1 + j) / chanels] == 0 && mask_p11[(dl1 + j) / chanels + 1] == 0 && mask_p11[(dl1 + j) / chanels + 2] == 0 &&
					mask_p12[(dl1 + j) / chanels] == 0 && mask_p12[(dl1 + j) / chanels + 1] == 0 && mask_p12[(dl1 + j) / chanels + 2] == 0 &&
					mask_p20[j / chanels] == 0 && mask_p20[j / chanels + 1] == 0 && mask_p20[j / chanels + 2] == 0 &&
					mask_p21[j / chanels] == 0 && mask_p21[j / chanels + 1] == 0 && mask_p21[j / chanels + 2] == 0 &&
					mask_p22[j / chanels] == 0 && mask_p22[j / chanels + 1] == 0 && mask_p22[j / chanels + 2] == 0) {
					if (img1 <= img2 + 10 &&
						img1 >= img2 - 10) {
						count++;
					}
					count_all++;
				}
			}
		}
		if (maxcount < (double)count / count_all) {
			maxcount = (double)count / count_all;
			relative_pos.x = l;
		}
	}
	return relative_pos;
}