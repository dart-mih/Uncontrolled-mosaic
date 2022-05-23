#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../Shots_normalization/PhotoAndCameraInf.h"
#include "../Shots_normalization/Normalization_function.h"

using namespace std;
using namespace cv;

/*
Finds the position of the camera after normalizing the image (because the rotations of the plane affect the position of the camera from which the given image could be taken).
img - structure that stores information about the image, the position of the camera after normalization on which we are looking.
camera_inf - structure containing camera information.
img_width - image width after normalization.
img_height - image height after normalization.
norm_distance - distance from the camera to the image used in normalization (in pixels).
*/
Point findPositionOfCameraAfterNormalization(PhotoInf& img, CameraInf& camera_inf,
    int img_width, int img_height, double norm_distance) {
    double resize_coeff = camera_inf.width / img_width;

    Mat rotation_matrix = getRotationMatrix3dTo2d(img.roll, img.pitch, img.yaw,
        camera_inf.center_x * resize_coeff, camera_inf.center_y * resize_coeff, norm_distance);

    Mat camera_start_vec = (Mat_<double>(4, 1) << img_width / 2, img_height / 2, norm_distance, 1);
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
Point getApproxRelativeDistOfPhotos(PhotoInf& first_photo, PhotoInf& second_photo, int x_first, int y_first,
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
first_img - image relative to which the position of the second is searched.
second_img - frame following the first image.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
camera_inf - structure containing camera information.
norm_distance - distance from the camera to the image used in normalization (in pixels).
path_norm_photos - path to normalized images.
positions_images - an allocated array that will contain after the algorithm the positions of each image relative to the first one.
*/
Point justGPSalg(Mat& first_img, Mat& second_img, PhotoInf& first_photo_inf, PhotoInf& second_photo_inf, 
    CameraInf& camera_inf, double norm_distance, string path_norm_photos, Rect* positions_images) {
    // Finding the position of the camera after rotating the first image.
    Point pos_camera_start_first = findPositionOfCameraAfterNormalization(first_photo_inf, camera_inf,
        first_img.cols, first_img.rows, norm_distance);

    // Finding the position of the camera after rotating the second image.
    Point pos_camera_start_second = findPositionOfCameraAfterNormalization(second_photo_inf, camera_inf,
        second_img.cols, second_img.rows, norm_distance);

    // Get the relative distance between the photos.
    Point relative_pos = getApproxRelativeDistOfPhotos(first_photo_inf, second_photo_inf, pos_camera_start_second.x,
        pos_camera_start_second.y, pos_camera_start_first.x, pos_camera_start_first.y);
    return relative_pos;
}

