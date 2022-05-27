#pragma once

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include "PhotoAndCameraInf.h"

using namespace std;
using namespace cv;


/*
Generates a matrix to convert a 2d vector of a homogeneous coordinate system (x, y, 1) to a 3d vector of a homogeneous coordinate system (x, y, 0, 1).
point_rotate_x - coordinate of the point relative to which the image is rotated (Ox).
point_rotate_y - coordinate of the point relative to which the image is rotated (Oy).
*/
Mat getMatrix2dto3d(double point_rotate_x, double point_rotate_y) {
    // Projection 2D -> 3D matrix.
    Mat A1 = (Mat_<double>(4, 3) <<
        1, 0, -point_rotate_x,
        0, 1, -point_rotate_y,
        0, 0, 0,
        0, 0, 1);
    return A1;
}

/*
Generates a matrix for rotating a 3d vector of a homogeneous coordinate system (x, y, z, 1).
alpha - rotation angle relative to Ox (in degrees).
beta - rotation angle relative to Oy (in degrees).
gamma - rotation angle relative to Oz (in degrees).
point_rotate_x - coordinate of the point relative to which the image is rotated.
point_rotate_y - coordinate of the point relative to which the image is rotated.
f - distance to the camera (in image pixels).
*/
Mat getRotationMatrix3dTo2d(double alpha, double beta, double gamma,
    double point_rotate_x, double point_rotate_y, double f) {
    alpha = alpha * CV_PI / 180.;
    beta = beta * CV_PI / 180.;
    gamma = gamma * CV_PI / 180.;

    // Rotation matrices around the X, Y, and Z axes.
    Mat RX = (Mat_<double>(4, 4) <<
        1, 0, 0, 0,
        0, cos(alpha), -sin(alpha), 0,
        0, sin(alpha), cos(alpha), 0,
        0, 0, 0, 1);
    Mat RY = (Mat_<double>(4, 4) <<
        cos(beta), 0, -sin(beta), 0,
        0, 1, 0, 0,
        sin(beta), 0, cos(beta), 0,
        0, 0, 0, 1);
    Mat RZ = (Mat_<double>(4, 4) <<
        cos(gamma), -sin(gamma), 0, 0,
        sin(gamma), cos(gamma), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    // General matrix of all rotations (RX, RY, RZ).
    Mat R = RX * RY * RZ;

    // Offset matrix.
    Mat T = (Mat_<double>(4, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, f,
        0, 0, 0, 1);

    // Back projection matrix 3D -> 2D.
    Mat A2 = (Mat_<double>(3, 4) <<
        f, 0, point_rotate_x, 0,
        0, f, point_rotate_y, 0,
        0, 0, 1, 0);

    // The matrix of the resulting transformation.
    return A2 * T * R;
}

/*
Rotates the image by the specified angles.
image - source image.
result - original image after rotation.
alpha - rotation angle relative to Ox (in degrees).
beta - rotation angle relative to Oy (in degrees).
gamma - rotation angle relative to Oz (in degrees).
point_rotate_x - coordinate of the point relative to which the image is rotated.
point_rotate_y - coordinate of the point relative to which the image is rotated.
f - distance to the camera (in image pixels).
*/
void rotateImage(const Mat& image, Mat& result, double alpha, double beta, double gamma,
    double point_rotate_x, double point_rotate_y, double f) {

    // The matrix of the resulting transformation.
    Mat final_trans_mat = getMatrix2dto3d(point_rotate_x, point_rotate_y);
    final_trans_mat = getRotationMatrix3dTo2d(alpha, beta, gamma, point_rotate_x, point_rotate_y, f) * final_trans_mat;

    // Applying a matrix transformation.
    warpPerspective(image, result, final_trans_mat, image.size(), INTER_LANCZOS4);
}

/*
Gets a very approximate distance from the camera to the shooting surface.
photo_inf - information about one drone frame.
next_photo_inf - information about the drone frame after photo_inf frame.
camera_shot_height - height of one image frame (in pixels).
*/
double getNormalizationDistance(PhotoInf& photo_inf, PhotoInf& next_photo_inf, int camera_shot_height) {
    // Approximately pixels in one meter.
    double pix_in_one_meter = (camera_shot_height * 0.3) / (abs(photo_inf.latitude - next_photo_inf.latitude) * 111412);
    // Approximate distance to the camera.
    double distance = photo_inf.altBaro * pix_in_one_meter;
    return distance;
}

/*
Normalizes one photo, rotating it to the required angles and writing it with the same name on the output path.
src - path where the original images are located.
output - path where the normalized images will be saved.
photo_inf - structure with information about photo.
camera_inf - structure containing camera information.
norm_distance - distance from the camera to the image used in normalization (in pixels).
distance_meters - distance to which the distance to the image is brought (same for all photos).
*/
void normalizeShot(string src, string output, PhotoInf& photo_inf, CameraInf& camera_inf, double norm_distance, double distance_meters) {
    Mat image = imread(src + photo_inf.name);

    float resize_coeff = photo_inf.altBaro / distance_meters;

    Mat resized_image;
    Mat normalized_image;

    printf("resize_coeff: %f\n", resize_coeff);

    resize(image, resized_image, Size((int)(image.cols * resize_coeff), (int)(image.rows * resize_coeff)), INTER_LINEAR);

    rotateImage(resized_image, normalized_image, photo_inf.roll, photo_inf.pitch, photo_inf.yaw,
        camera_inf.center_x * resize_coeff, camera_inf.center_y * resize_coeff, norm_distance);


    imwrite(output + photo_inf.name, normalized_image);
}