#pragma once

#include <string>

using namespace std;

/*
���������� � ������ �����.
name - �������� ������ �����������.
latitude - ������� ������.
longitude - ������ ������.
altBaro - ������ ������ � ������������ � ����������� ��������� (�����).
roll - ���� ������� ������ ��� ������ � ��������� ������� ���������.
pitch - ���� ������� ������ ��� ������ � ��������� ������� ���������.
yaw - ���� ������� ������ ��� ������ � ��������� ������� ���������.
time - ����� ������ �����.
altGPS - ������ ������ � ������������ � ����������� GPS (�����)..
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
���������� � ������.
OPENCV_FULL camera parameters:
camera_id - ���������� ������������� ����� ����� ��������� ���� �����.
model == OPENCV_FULL
width - ������ ����� � ��������.
height - ������ ����� � ��������.
fx - ��������� ���������� 1.
fy - ��������� ���������� 2.
center_x - �������� ������� ������ ������ �� ����������.
center_y - �������� ������� ������ ������ �� ����������.
��� ��������� ��������� - ������������ ��������� ������������, ��.:
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