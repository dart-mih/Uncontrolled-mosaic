#pragma once

#include <string>

using namespace std;

/*
Информация о снятом кадре.
name - название самого изображения.
latitude - долгота съемки.
longitude - широта съемки.
altBaro - высота съемки в соответствии с показаниями барометра (метры).
roll - угол наклона камеры при съемке в связанной системе координат.
pitch - угол наклона камеры при съемке в связанной системе координат.
yaw - угол наклона камеры при съемке в связанной системе координат.
time - время съемки кадра.
altGPS - высота съемки в соответствии с показаниями GPS (метры)..
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
Информация о камере.
OPENCV_FULL camera parameters:
camera_id - уникальный идентификатор среди камер снимавших одну сцену.
model == OPENCV_FULL
width - ширина кадра в пикселях.
height - высота кадра в пикселях.
fx - фокальное расстояние 1.
fy - фокальное расстояние 2.
center_x - реальная позиция центра камеры на устройстве.
center_y - реальная позиция центра камеры на устройстве.
Все остальные параметры - коэффициенты искажения фотоаппарата, см.:
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