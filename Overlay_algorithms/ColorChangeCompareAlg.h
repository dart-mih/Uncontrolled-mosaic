#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../Shots_normalization/PhotoAndCameraInf.h"
#include "PixelCompareAlg.h"

using namespace std;
using namespace cv;

Mat imgThreshold(Mat& img) {
	Mat greyScaleImg;
	cvtColor(img, greyScaleImg, COLOR_BGR2GRAY);
	Mat output;
	adaptiveThreshold(greyScaleImg, output, 1, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 13, 2);
	return output;
}

/*
Algorithm for matching images by pixel comparision of greyscale variants of images.
first_img - image relative to which the position of the second is searched.
second_img - frame following the first image.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
vertical_shift - in which interval relative to the center_search_pos to look for the offset along Oy.
horizontal_shift - in which interval relative to the center_search_pos to look for the offset along Ox.
center_search_pos - center of the area in which we are looking for the offset of the second image left top corner relative to the first image corner.
*/
Point greyscaleCompareAlg(Mat& first_img, Mat& second_img, PhotoInf& first_photo_inf, PhotoInf& second_photo_inf,
	int vertical_shift, int horizontal_shift = 50, Point center_search_pos = Point{ 0, 0 }) {
	int count = 0;
	int maxcount = 0;

	Mat first_img_greyscale;
	Mat second_img_greyscale;

	cvtColor(first_img, first_img_greyscale, COLOR_BGR2GRAY);
	cvtColor(second_img, second_img_greyscale, COLOR_BGR2GRAY);

	Point relative_pos = pixelCompareAlgOneChanel(first_img_greyscale, second_img_greyscale, first_photo_inf, second_photo_inf,
		vertical_shift, horizontal_shift, center_search_pos);
	return relative_pos;
}

/*
Algorithm for matching images by pixel comparision of hsv variants of images.
first_img - image relative to which the position of the second is searched.
second_img - frame following the first image.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
vertical_shift - in which interval relative to the center_search_pos to look for the offset along Oy.
horizontal_shift - in which interval relative to the center_search_pos to look for the offset along Ox.
center_search_pos - center of the area in which we are looking for the offset of the second image left top corner relative to the first image corner.
*/
Point hsvCompareAlg(Mat& first_img, Mat& second_img, PhotoInf& first_photo_inf, PhotoInf& second_photo_inf,
	int vertical_shift, int horizontal_shift = 50, Point center_search_pos = Point{ 0, 0 }) {
	int count = 0;
	int maxcount = 0;

	Mat first_img_hsv;
	Mat second_img_hsv;

	cvtColor(first_img, first_img_hsv, COLOR_BGR2HSV);
	cvtColor(second_img, second_img_hsv, COLOR_BGR2HSV);

	Point relative_pos = pixelCompareAlg(first_img_hsv, second_img_hsv, first_photo_inf, second_photo_inf,
		vertical_shift, horizontal_shift, center_search_pos);
	return relative_pos;
}