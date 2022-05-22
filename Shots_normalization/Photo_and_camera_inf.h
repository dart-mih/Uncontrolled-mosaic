#pragma once

#include <string>

using namespace std;

/*
Information about the captured frame.
name - the name of the image itself.
latitude - longitude of the survey.
longitude - survey latitude.
altBaro - shooting height according to barometer readings (meters).
roll - camera tilt angle when shooting in the linked coordinate system.
pitch - camera tilt angle when shooting in the linked coordinate system.
yaw - camera tilt angle when shooting in the linked coordinate system.
time - the time the frame was taken.
altGPS - shooting height according to GPS readings (meters).
*/
struct PhotoInf {
    string name;
    double latitude;
    double longitude;
    double altBaro;

    double roll;
    double pitch;
    double yaw;

    string time;

    double altGPS;
};

/*
Information about the camera.
OPENCV_FULL camera parameters:
camera_id - a unique identifier among cameras filming the same scene.
model == OPENCV_FULL
width - frame width in pixels.
height - frame height in pixels.
fx - focal length 1.
fy - focal length 2.
center_x - actual position of the camera center on the device.
center_y - actual position of the camera center on the device.
All other parameters are camera distortion coefficients, see:
https://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
*/
struct CameraInf {
    int camera_id;
    string model;
    double width;
    double height;

    double fx;
    double fy;
    double center_x;
    double center_y;
    double k1;
    double k2;
    double p1;
    double p2;
    double k3;
    double k4;
    double k5;
    double k6;
};