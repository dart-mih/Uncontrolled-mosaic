#include <iostream>
#include <opencv2/opencv.hpp>

#include "Shots_normalization/PhotoAndCameraInf.h"

using namespace std;
using namespace cv;

/*
A function that combines all images into one canvas at the coordinates specified in find_positions_images.
num_photos - number of photos.
num_first_broken_photos - number of first pictures that cannot be used for overlay (it is also the index of the first one that can be used).
photos_inf - array of structures containing information about photos.
path_norm_photos - path to normalized images.
find_positions_images - array that contains the positions of each image relative to the first one.
res_img_path - the path where the result of the combination will be located.
*/
void combinePhotos(int num_photos, int num_first_broken_photos, PhotoInf* photos_inf, string path_norm_photos,
    Rect* find_positions_images, string res_img_path) {
    // Factor to replace black pixels in the final canvas.
    int black_pixel_catch_coeff = 5;

    // Find the canvas size.
    Point res_img_left_top = Point(100000, 1000000);
    Point res_img_right_down = Point(-100000, -1000000);

    for (int i = 0; i < num_photos - num_first_broken_photos; i++) {
        if (find_positions_images[i].x < res_img_left_top.x) {
            res_img_left_top.x = find_positions_images[i].x;
        }
        if (find_positions_images[i].y < res_img_left_top.y) {
            res_img_left_top.y = find_positions_images[i].y;
        }
        if (find_positions_images[i].x + find_positions_images[i].width > res_img_right_down.x) {
            res_img_right_down.x = find_positions_images[i].x + find_positions_images[i].width;
        }
        if (find_positions_images[i].y + find_positions_images[i].height > res_img_right_down.y) {
            res_img_right_down.y = find_positions_images[i].y + find_positions_images[i].height;
        }
    }

    // Move all the pictures.
    for (int i = 0; i < num_photos - num_first_broken_photos; i++) {
        find_positions_images[i] -= res_img_left_top;
    }

    // Now find the size of the canvas and create it.
    int res_width = res_img_right_down.x - res_img_left_top.x;
    int res_height = res_img_right_down.y - res_img_left_top.y;

    Mat one_img = imread(path_norm_photos + photos_inf[num_first_broken_photos].name);
    Mat result = Mat(res_height, res_width, one_img.type(), Scalar(0, 0, 0));

    int num_channels = one_img.channels();

    // Fill the canvas with images.
    uchar* p_img;
    uchar* p_res;
    for (int i = 0; i < num_photos - num_first_broken_photos; i++) {
        Mat img = imread(path_norm_photos + photos_inf[i + num_first_broken_photos].name);
        for (int y = 0; y < img.rows; y++) {
            p_img = img.ptr<uchar>(y);
            p_res = result.ptr<uchar>(y + find_positions_images[i].y);
            for (int x = 0; x < img.cols; x++) {
                if (!(p_img[num_channels * x] <= black_pixel_catch_coeff && p_img[num_channels * x + 1] <= black_pixel_catch_coeff
                    && p_img[num_channels * x + 2] <= black_pixel_catch_coeff)) {
                    p_res[num_channels * (find_positions_images[i].x + x)] = p_img[num_channels * x];
                    p_res[num_channels * (find_positions_images[i].x + x) + 1] = p_img[num_channels * x + 1];
                    p_res[num_channels * (find_positions_images[i].x + x) + 2] = p_img[num_channels * x + 2];
                }
            }
        }
    }

    imwrite(res_img_path, result);
}

/*
A function that combines all images into one canvas at the coordinates specified in find_positions_images.
All except the last image are not drawn in full size, but only up to the height of the best comparison x2.
num_photos - number of photos.
num_first_broken_photos - number of first pictures that cannot be used for overlay (it is also the index of the first one that can be used).
photos_inf - array of structures containing information about photos.
path_norm_photos - path to normalized images.
find_positions_images - array that contains the positions of each image relative to the first one.
res_img_path - the path where the result of the combination will be located.
*/
void combinePhotosOptimized(int num_photos, int num_first_broken_photos, PhotoInf* photos_inf, string path_norm_photos,
    Rect* find_positions_images, string res_img_path) {
    // Factor to replace black pixels in the final canvas.
    int black_pixel_catch_coeff = 5;

    // Find the canvas size.
    Point res_img_left_top = Point(100000, 1000000);
    Point res_img_right_down = Point(-100000, -1000000);;

    for (int i = 0; i < num_photos - num_first_broken_photos; i++) {
        if (find_positions_images[i].x < res_img_left_top.x) {
            res_img_left_top.x = find_positions_images[i].x;
        }
        if (find_positions_images[i].y < res_img_left_top.y) {
            res_img_left_top.y = find_positions_images[i].y;
        }
        if (find_positions_images[i].x + find_positions_images[i].width > res_img_right_down.x) {
            res_img_right_down.x = find_positions_images[i].x + find_positions_images[i].width;
        }
        if (find_positions_images[i].y + find_positions_images[i].height > res_img_right_down.y) {
            res_img_right_down.y = find_positions_images[i].y + find_positions_images[i].height;
        }
    }

    // Move all the pictures.
    for (int i = 0; i < num_photos - num_first_broken_photos; i++) {
        find_positions_images[i] -= res_img_left_top;
    }

    // Now find the size of the canvas and create it.
    int res_width = res_img_right_down.x - res_img_left_top.x;
    int res_height = res_img_right_down.y - res_img_left_top.y;

    Mat one_img = imread(path_norm_photos + photos_inf[num_first_broken_photos].name);
    Mat result = Mat(res_height, res_width, one_img.type(), Scalar(0, 0, 0));

    int num_channels = one_img.channels();

    uchar* p1;
    uchar* p2;

    //The very first image is written first.
    for (int i = 0; i < find_positions_images[1].y; i++) {
        p1 = result.ptr<uchar>(i);
        p2 = one_img.ptr<uchar>(i);
        for (int j = 0; j < one_img.cols * num_channels; j++) {
            p1[j] = p2[j];
        }
    }

    int res_offset_x = find_positions_images[0].x;
    int res_offset_y = find_positions_images[0].y;

    /*then the rest are written, except for the last*/
    for (int k = 1; k < num_photos - num_first_broken_photos - 1; k++) {
        one_img = imread(path_norm_photos + photos_inf[num_first_broken_photos + k].name);

        res_offset_x = find_positions_images[k].x;
        res_offset_y = find_positions_images[k].y;

        int draw_height = min(one_img.rows, abs(2*(find_positions_images[k].y - find_positions_images[k - 1].y)));

        for (int i = 0; i < draw_height; i++) {
            p1 = result.ptr<uchar>(i + res_offset_y);
            p2 = one_img.ptr<uchar>(i);
            for (int j = 0; j < one_img.cols * num_channels; j += 3) {
                if (!(p2[j] <= black_pixel_catch_coeff && p2[j + 1] <= black_pixel_catch_coeff && p2[j + 2] <= black_pixel_catch_coeff)) {
                    p1[res_offset_x * num_channels + j] = p2[j];
                    p1[res_offset_x * num_channels + j + 1] = p2[j + 1];
                    p1[res_offset_x * num_channels + j + 2] = p2[j + 2];
                }
            }
        }
    }

    one_img = imread(path_norm_photos + photos_inf[num_photos - 1].name);

    res_offset_x = find_positions_images[num_photos - num_first_broken_photos - 1].x;
    res_offset_y = find_positions_images[num_photos - num_first_broken_photos - 1].y;

    // Recording the last image in full.
    for (int i = 0; i < one_img.rows; i++) {
        p1 = result.ptr<uchar>(i + res_offset_y);
        p2 = one_img.ptr<uchar>(i);
        for (int j = 0; j < one_img.cols * num_channels; j += 3) {
            if (i + res_offset_y >= result.rows || j >= result.cols * num_channels) {
                cout << "jopa " << i + res_offset_y << " - " << result.rows << ", " << j + res_offset_x << " - " << result.cols << endl;
            }
            if (!(p2[j] <= black_pixel_catch_coeff && p2[j + 1] <= black_pixel_catch_coeff && p2[j + 2] <= black_pixel_catch_coeff)) {
                p1[res_offset_x * num_channels + j] = p2[j];
                p1[res_offset_x * num_channels + j + 1] = p2[j + 1];
                p1[res_offset_x * num_channels + j + 2] = p2[j + 2];
            }
        }
    }
    imwrite(res_img_path, result);
}