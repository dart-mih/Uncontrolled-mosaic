#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../Shots_normalization/PhotoAndCameraInf.h"
#include "../Shots_normalization/NormalizationFunction.h"

using namespace std;
using namespace cv;

/*
Find position of image according to camera coordinates.
Returns position of left top corner.
roll - rotation angle relative to Ox (in degrees).
pitch - rotation angle relative to Oy (in degrees).
yaw - rotation angle relative to Oz (in degrees).
pos_camera - camera position (x, y, z) in pixels.
camera_angles - camera view angles along the Ox and Oy axis (in degrees).
*/
Point findRelativePos(float roll, float pitch, float yaw,
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

    Point2f min_p = dst[0];
    for (int i = 0; i < 4; i++) {
        min_p.y = min(min_p.y, dst[i].y);
        min_p.x = min(min_p.x, dst[i].x);
    }

    return min_p;
}

/*
Algorithm for matching images by GPS coordinates of the camera at the time of shooting.
first_img_height - first image height.
second_img_height - second image height.
first_photo_inf - structure with information about first picture.
second_photo_inf - structure with information about second picture.
camera_inf - structure containing camera information.
*/
Point justGPSalg(int first_img_height, int second_img_height, PhotoInf& first_photo_inf, PhotoInf& second_photo_inf, 
    CameraInf& camera_inf) {
    Point2d camera_angles = getCameraAngles(camera_inf);

    double lattitude_to_pixels = lattitudeToMetersCoeff(first_photo_inf.latitude) *
        metersToPixelsCoeff(first_img_height, first_photo_inf.altBaro, camera_angles);
    double longitude_to_pixels = longitudeToMetersCoeff(first_photo_inf.longitude) *
        metersToPixelsCoeff(first_img_height, first_photo_inf.altBaro, camera_angles);

    // Relative position calculation.
    double lattitude_diff = second_photo_inf.latitude - first_photo_inf.latitude;
    double longitude_diff = second_photo_inf.longitude - first_photo_inf.longitude;

    // Set the position of the camera.
    Point3f pos_first_camera(0,
        metersToPixelsCoeff(first_img_height, first_photo_inf.altBaro, camera_angles) * first_photo_inf.altBaro,
        0);
    Point3f pos_second_camera(lattitude_diff * lattitude_to_pixels,
        metersToPixelsCoeff(second_img_height, second_photo_inf.altBaro, camera_angles) * second_photo_inf.altBaro,
        longitude_diff * longitude_to_pixels);

    // Get the relative distance between the photos.
    Point pos_first = findRelativePos(first_photo_inf.roll, first_photo_inf.pitch, first_photo_inf.yaw, pos_first_camera, camera_angles);
    Point pos_second = findRelativePos(second_photo_inf.roll, second_photo_inf.pitch, second_photo_inf.yaw, pos_second_camera, camera_angles);
    //return pos_second;
    return pos_second - pos_first;
}

