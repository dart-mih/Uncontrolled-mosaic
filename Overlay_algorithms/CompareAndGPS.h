#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../Shots_normalization/PhotoAndCameraInf.h"
#include "../Shots_normalization/Normalization_function.h"
#include "PixelCompareAlg.h"
#include "JustGPSalg.h"

using namespace std;
using namespace cv;

/*
Algorithm for matching images by GPS and refining the position using pixelCompareAlg.
first_img - image relative to which the position of the second is searched.
second_img - frame following the first image.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
vertical_shift - in which interval relative to the center_search_pos to look for the offset along Oy.
horizontal_shift - in which interval relative to the center_search_pos to look for the offset along Ox.
*/
Point compareAndGPSalg(Mat& first_img, Mat& second_img, PhotoInf& first_photo_inf, PhotoInf& second_photo_inf,
	CameraInf& camera_inf, double norm_distance, int vertical_shift = 300, int horizontal_shift = 300) {
	int count = 0;
	int maxcount = 0;

	Point relative_pos = justGPSalg(first_img, second_img, first_photo_inf, second_photo_inf, camera_inf, norm_distance);

	relative_pos = pixelCompareAlg(first_img, second_img, first_photo_inf, second_photo_inf,
		vertical_shift, horizontal_shift, relative_pos);
	return relative_pos;
}