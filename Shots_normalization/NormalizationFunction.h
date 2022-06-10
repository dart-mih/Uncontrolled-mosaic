#pragma once

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PhotoAndCameraInf.h"

using namespace std;
using namespace cv;

/*
Gets the number of meters in one degree of latitude for the given latitude.
*/
double lattitudeToMetersCoeff(double lattitude) {
    return 110480;
}

/*
Gets the number of meters in one degree of longitude for the given longitude.
*/
double longitudeToMetersCoeff(double longitude) {
    return 55654;
}

/*
Gets the coefficient for converting meters to image pixels.
image_height - image frame height (in pixels).
camera_height - camera height at the time of the shot (in meters).
camera_angles - camera view angles along the Ox and Oy axis (in degrees).
*/
double metersToPixelsCoeff(int image_height, double camera_height, Point2d camera_angles) {
    double angle_y = camera_angles.y * CV_PI / 180.;
    return image_height / (2 * camera_height * tan(angle_y / 2));
}

/*
Gets the camera view angle along the Ox and Oy axes (in degrees).
camera_inf - structure containing camera information.
*/
Point2f getCameraAngles(CameraInf& camera_inf) {
    return Point2d(360 * atan2(camera_inf.width, 2 * camera_inf.fx) / CV_PI, 360 * atan2(camera_inf.height, 2*camera_inf.fy) / CV_PI);
    //return Point2d(3 * 14.4298679, 14.4298679 * 2);
}

/*
Rotates the image by the specified angles.
image - source image.
result - original image after rotation.
roll - rotation angle relative to Ox (in degrees).
pitch - rotation angle relative to Oy (in degrees).
yaw - rotation angle relative to Oz (in degrees).
pos_camera - camera position (x, y, z) in pixels.
camera_angles - camera view angles along the Ox and Oy axis (in degrees).
*/
void rotateImage(const Mat& image, Mat& result, float roll, float pitch, float yaw,
    Point3f pos_camera, Point2f camera_angles) {
    //axis:
    // 0 - по направлению
    // 1 - вверх
    // 2 - вправо
    glm::vec3 axis[3];
    axis[0] = glm::vec3(1.0f, 0.0f, 0.0f);
    axis[1] = glm::vec3(0.0f, 1.0f, 0.0f);
    axis[2] = glm::vec3(0.0f, 0.0f, 1.0f);

    auto mat_rot = glm::rotate(glm::mat4(1.0f), glm::radians(-yaw), axis[1]);
    auto mat_rot3 = glm::mat3(mat_rot);
    axis[0] = mat_rot3 * axis[0];
    axis[2] = mat_rot3 * axis[2];

    mat_rot = glm::rotate(glm::mat4(1.0f), glm::radians(pitch), axis[2]);
    mat_rot3 = glm::mat3(mat_rot);
    axis[0] = mat_rot3 * axis[0];
    axis[1] = mat_rot3 * axis[1];

    mat_rot = glm::rotate(glm::mat4(1.0f), glm::radians(-roll), axis[0]);
    mat_rot3 = glm::mat3(mat_rot);
    axis[2] = mat_rot3 * axis[2];
    axis[1] = mat_rot3 * axis[1];


    glm::vec3 camera_line = axis[1] * -3.0f;

    glm::vec3 frame_rays[4];
    frame_rays[0] = glm::mat3(glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(camera_angles.y / 2), axis[2]), glm::radians(camera_angles.x / 2), axis[0])) * camera_line * 500.0f;
    frame_rays[1] = glm::mat3(glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(camera_angles.y / 2), axis[2]), glm::radians(-camera_angles.x / 2), axis[0])) * camera_line * 500.0f;
    frame_rays[2] = glm::mat3(glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-camera_angles.y / 2), axis[2]), glm::radians(-camera_angles.x / 2), axis[0])) * camera_line * 500.0f;
    frame_rays[3] = glm::mat3(glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-camera_angles.y / 2), axis[2]), glm::radians(camera_angles.x / 2), axis[0])) * camera_line * 500.0f;

    glm::vec3 dots[4];
    for (int i = 0; i < 4; i++) {
        dots[i] = glm::normalize(frame_rays[i]);
        float coeff = -pos_camera.y / dots[i].y;
        dots[i].x = coeff * dots[i].x + pos_camera.x;
        dots[i].z = coeff * dots[i].z + pos_camera.z;
    }

    // Get the true angles of the image.
    Point2f dst[4];

    for (int i = 0; i < 4; i++) {
        dst[i].y = dots[i].x;
        dst[i].x = dots[i].z;
    }

    // The original angles of the image.
    Point2f src[4];

    src[0] = { 0, 0 };
    src[1] = { 6000, 0 };
    src[2] = { 6000, 4000 };
    src[3] = { 0, 4000 };

    // Change the size and angles so that the image fits entirely.
    Point2f min_p = dst[0];
    Point2f max_p = dst[3];
    for (int i = 0; i < 4; i++) {
        min_p.y = min(min_p.y, dst[i].y);
        min_p.x = min(min_p.x, dst[i].x);
        max_p.y = max(max_p.y, dst[i].y);
        max_p.x = max(max_p.x, dst[i].x);
    }

    for (int i = 0; i < 4; i++) {
        dst[i] -= min_p;
    }

    Mat transform_mat = getPerspectiveTransform(src, dst, cv::INTER_LINEAR);

    // Applying a matrix transformation.
    warpPerspective(image, result, transform_mat, Size(max_p - min_p), INTER_LANCZOS4);
}

/*
Normalizes one photo, rotating it to the required angles and writing it with the same name on the output path.
image - image to normalize.
result - result of normalization will be stored where.
photo_inf - structure with information about photo.
camera_inf - structure containing camera information.
*/
void normalizeShot(Mat& image, Mat& result, PhotoInf& photo_inf, CameraInf& camera_inf) {
    // Get camera angles.
    Point2d camera_angles = getCameraAngles(camera_inf);

    // Set the position of the camera.
    Point3f pos_camera(0, 
        metersToPixelsCoeff(image.rows, photo_inf.altBaro, camera_angles) * photo_inf.altBaro, 
        0);

    rotateImage(image, result, photo_inf.roll, photo_inf.pitch, photo_inf.yaw,
        pos_camera, camera_angles);
}
