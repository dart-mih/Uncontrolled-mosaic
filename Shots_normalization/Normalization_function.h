#pragma once

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include "Photo_and_camera_inf.h"

using namespace std;
using namespace cv;


/*
���������� ������� ��� �������������� 2d ������� ���������� ������� ��������� (x, y, 1) � ������ 3d ���������� ������� ��������� (x, y, 0, 1).
point_rotate_x - ���������� �����, ������������ ������� ���������� ������� ��������.
point_rotate_y - ���������� �����, ������������ ������� ���������� ������� ��������.
*/
Mat getMatrix2dto3d(double point_rotate_x, double point_rotate_y) {
    // �������� 2D -> 3D ������.
    Mat A1 = (Mat_<double>(4, 3) <<
        1, 0, -point_rotate_x,
        0, 1, -point_rotate_y,
        0, 0, 0,
        0, 0, 1);
    return A1;
}

/*
���������� ������� ��� �������� 3d ������� ���������� ������� ��������� (x, y, z, 1).
alpha - ���� �������� ������������ Ox (� ��������).
beta  - ���� �������� ������������ Oy (� ��������).
gamma - ���� �������� ������������ Oz (� ��������).
point_rotate_x - ���������� �����, ������������ ������� ���������� ������� ��������.
point_rotate_y - ���������� �����, ������������ ������� ���������� ������� ��������.
f - ���������� �� ������ (� �������� �����������).
*/
Mat getRotationMatrix3dTo2d(double alpha, double beta, double gamma,
    double point_rotate_x, double point_rotate_y, double f) {
    alpha = alpha * CV_PI / 180.;
    beta = beta * CV_PI / 180.;
    gamma = gamma * CV_PI / 180.;

    // ������� �������� ������������ ���� X, Y, � Z.
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

    // ����� ������� ���� ��������� (RX, RY, RZ).
    Mat R = RX * RY * RZ;

    // ������� �������� 
    Mat T = (Mat_<double>(4, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, f,
        0, 0, 0, 1);

    // �������� �������� ������ 3D -> 2D.
    Mat A2 = (Mat_<double>(3, 4) <<
        f, 0, point_rotate_x, 0,
        0, f, point_rotate_y, 0,
        0, 0, 1, 0);

    // ������� �������������� �������������.
    return A2 * T * R;
}

/*
������������ ����������� �� �������� ����.
image - �������� ��������.
result - �������� �������� ����� ��������.
alpha - ���� �������� ������������ Ox (� ��������).
beta  - ���� �������� ������������ Oy (� ��������).
gamma - ���� �������� ������������ Oz (� ��������).
point_rotate_x - ���������� �����, ������������ ������� ���������� ������� ��������.
point_rotate_y - ���������� �����, ������������ ������� ���������� ������� ��������.
f - ���������� �� ������ (� �������� �����������).
*/
void rotateImage(const Mat& image, Mat& result, double alpha, double beta, double gamma,
    double point_rotate_x, double point_rotate_y, double f) {

    // ������� �������������� �������������.
    Mat final_trans_mat = getMatrix2dto3d(point_rotate_x, point_rotate_y);
    final_trans_mat = getRotationMatrix3dTo2d(alpha, beta, gamma, point_rotate_x, point_rotate_y, f) * final_trans_mat;

    // ��������� ��������� �������������
    warpPerspective(image, result, final_trans_mat, image.size(), INTER_LANCZOS4);
}

/*
�������� ����� ��������������� ���������� �� ������ �� ����������� ������.
photo_inf - ���������� �� ����� ����� �����.
next_photo_inf - ���������� � ����� ����� ����� photo_inf.
camera_shot_height - ������ ������ ����� ����������� (� ��������).
*/
double getNormalizationDistance(PhotoInf& photo_inf, PhotoInf& next_photo_inf, int camera_shot_height) {
    double pix_in_one_meter = (camera_shot_height * 0.3) / (abs(photo_inf.latitude - next_photo_inf.latitude) * 111412); // �������� �������� � ����� �����.
    double distance = photo_inf.altBaro * pix_in_one_meter; // ��������� ��������� �� ������.
    return distance;
}

void normalizeShots(string src, string output, PhotoInf* photos_inf, CameraInf& camera_inf,
    int num_photos, int num_first_broken_photos) {
    // �������� ����������, ������������ ����������� ������� �� ����� ��������� ��������� �����������;

    PhotoInf correcting_info = photos_inf[num_first_broken_photos];

    double distance = getNormalizationDistance(correcting_info, photos_inf[num_first_broken_photos + 1], camera_inf.height);

    for (int i = num_first_broken_photos; i < num_photos; i++) {
        Mat image = imread(src + photos_inf[i].name);

        float resize_coeff = photos_inf[i].altBaro / correcting_info.altBaro;

        Mat resized_image;
        Mat normalized_image;

        printf("resize_coeff: %f\n", resize_coeff);

        resize(image, resized_image, Size((int)(image.cols * resize_coeff), (int)(image.rows * resize_coeff)), INTER_LINEAR);

        rotateImage(resized_image, normalized_image, photos_inf[i].roll,
            photos_inf[i].pitch, photos_inf[i].yaw,
            camera_inf.center_x * resize_coeff, camera_inf.center_y * resize_coeff, distance);


        imwrite(output + photos_inf[i].name, normalized_image);
    }
}
