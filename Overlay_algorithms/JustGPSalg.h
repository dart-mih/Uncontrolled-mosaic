#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../Shots_normalization/Photo_and_camera_inf.h"
#include "../Shots_normalization/Normalization_function.h"

using namespace std;
using namespace cv;

/*
Finds the position of the camera after normalizing the image (because the rotations of the plane affect the position of the camera from which the given image could be taken).
num_first_broken_photos - number of first pictures that cannot be used for overlay (it is also the index of the first one that can be used).
photos_inf - array of structures containing information about photos.
camera_inf - structure containing camera information.
img_ind - index of the image in photos_inf, the position of the camera on which we are looking.
img_width - image width after normalization.
img_height - image height after normalization.
*/
Point findPositionOfCameraAfterNormalization(int num_first_broken_photos, PhotoInf* photos_inf, CameraInf& camera_inf,
    int img_ind, int img_width, int img_height) {
    double distance = getNormalizationDistance(photos_inf[num_first_broken_photos], photos_inf[num_first_broken_photos + 1], 
        camera_inf.height);
    double resize_coeff = camera_inf.width / img_width;

    Mat rotation_matrix = getRotationMatrix3dTo2d(photos_inf[img_ind].roll,
        photos_inf[img_ind].pitch, photos_inf[img_ind].yaw,
        camera_inf.center_x * resize_coeff, camera_inf.center_y * resize_coeff, distance);

    Mat camera_start_vec = (Mat_<double>(4, 1) << img_width / 2, img_height / 2, distance, 1);
    Mat camera_after_vec = rotation_matrix * camera_start_vec;

    Point camera_after_pos = Point(camera_after_vec.at<double>(0) / camera_after_vec.at<double>(2),
        camera_after_vec.at<double>(1) / camera_after_vec.at<double>(2));
    return camera_after_pos;
}

/*
Finds the relative distance between photos.
first_photo - the first photo we are looking for the distance from.
second_photo - second photo whose position relative to the first we are looking for.
x_first - camera position on the first photo by Ox.
y_first - camera position on the first photo by Oy.
x_second - camera position on the second photo by Ox.
y_second - camera position on the second photo by Oy.
*/
Point getApproxRelativeDistOfPhotos(PhotoInf first_photo, PhotoInf second_photo, int x_first, int y_first,
    int x_second, int y_second) {
    double lattitude_to_m_coeff = 111412;
    double longitude_to_m_coeff = 96486;

    double lattitude_to_pixels = -3330687.8306878;
    double longitude_to_pixels = (lattitude_to_pixels / lattitude_to_m_coeff) * longitude_to_m_coeff;

    // Relative position calculation.
    double lattitude_diff = second_photo.latitude - first_photo.latitude;
    double longitude_diff = -(second_photo.longitude - first_photo.longitude);
    Point relative_pos = Point(x_first + longitude_diff * longitude_to_pixels,
        y_first + lattitude_diff * lattitude_to_pixels);
    relative_pos -= Point(x_second, y_second);

    return relative_pos;
}

/*
Algorithm for matching images by GPS coordinates of the camera at the time of shooting.
num_photos - number of photos.
num_first_broken_photos - number of first pictures that cannot be used for overlay (it is also the index of the first one that can be used).
photos_inf - array of structures containing information about photos.
camera_inf - structure containing camera information.
path_norm_photos - path to normalized images.
positions_images - an allocated array that will contain after the algorithm the positions of each image relative to the first one.
*/
void justGPSalg(int num_photos, int num_first_broken_photos, PhotoInf* photos_inf, CameraInf& camera_inf, string path_norm_photos, Rect* positions_images) {
    Mat first_img = imread(path_norm_photos + photos_inf[num_first_broken_photos].name);
    positions_images[0] = Rect(Point(0, 0), Point(first_img.cols, first_img.rows));

    // Finding the position of the camera after rotating the image.
    Point pos_camera_start_first = findPositionOfCameraAfterNormalization(num_first_broken_photos, photos_inf, camera_inf,
        num_first_broken_photos, first_img.cols, first_img.rows);

    for (int i = num_first_broken_photos + 1; i < num_photos; i++) {
        Mat second_img = imread(path_norm_photos + photos_inf[i].name);

        // Finding the position of the camera after rotating the image.
        Point pos_camera_start_second = findPositionOfCameraAfterNormalization(num_first_broken_photos, photos_inf, camera_inf,
            i, first_img.cols, first_img.rows);

        // Get the relative distance between the photos.
        Point relative_pos = getApproxRelativeDistOfPhotos(photos_inf[i - 1], photos_inf[i], pos_camera_start_second.x,
            pos_camera_start_second.y, pos_camera_start_first.x, pos_camera_start_first.y);

        printf("Camera start pos first image %d is (%d, %d)]\n", i, pos_camera_start_first.x, pos_camera_start_first.y);
        printf("Camera start pos second image %d is (%d, %d)]\n", i + 1, pos_camera_start_second.x, pos_camera_start_second.y);

        // Fill the array of positions.
        positions_images[i - num_first_broken_photos] = Rect(positions_images[i - num_first_broken_photos - 1].x + relative_pos.x,
            positions_images[i - num_first_broken_photos - 1].y + relative_pos.y, second_img.cols, second_img.rows);

        printf("Relative pos of second (%d) image relatively first (%d): is x = %d, y = %d \n", i + 1, i, relative_pos.x, relative_pos.y);

        first_img = second_img;
        pos_camera_start_first = pos_camera_start_second;
    }
}

