#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../Shots_normalization/PhotoAndCameraInf.h"
#include "../Shots_normalization/NormalizationFunction.h"
#include "PixelCompareAlg.h"
#include "JustGPSalg.h"

using namespace std;
using namespace cv;

/*
Algorithm for matching images by GPS and refining the position using pixelCompareAlg.
first_img - image relative to which the position of the second is searched.
second_img - frame following the first image.
mask_first - mask, values 255 in which correspond to pixels that will not be taken into account when working with the first image.
mask_second - mask, values 255 in which correspond to pixels that will not be taken into account when working with the second picture.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
camera_inf - structure containing camera information.
vertical_shift - in which interval relative to the center_search_pos to look for the offset along Oy.
horizontal_shift - in which interval relative to the center_search_pos to look for the offset along Ox.
*/
Point compareAndGPSalg(Mat& first_img, Mat& second_img, Mat& mask_first, Mat& mask_second, PhotoInf& first_photo_inf, PhotoInf& second_photo_inf,
	CameraInf& camera_inf, double resize_ratio, int vertical_shift = 500, int horizontal_shift = 500) {
	int count = 0;
	int maxcount = 0;

	Point relative_pos = justGPSalg(first_img.rows * (1. / resize_ratio), second_img.rows * (1. / resize_ratio), 
		first_photo_inf, second_photo_inf, camera_inf);

	relative_pos.x = relative_pos.x * resize_ratio;
	relative_pos.y = relative_pos.y * resize_ratio;

	relative_pos = pixelCompareAlg(first_img, second_img, mask_first, mask_second, first_photo_inf, second_photo_inf,
		vertical_shift, horizontal_shift, relative_pos);
	return relative_pos;
}