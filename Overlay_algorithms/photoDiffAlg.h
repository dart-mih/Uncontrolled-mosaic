#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

#include "../Shots_normalization/Photo_and_camera_inf.h"

using namespace std;
using namespace cv;

void photoDiffAlg(int num_photos, int num_first_broken_photos, PhotoInf* photos_inf, string path_norm_photos, Rect* positions_images) {
    Mat first_img = imread(path_norm_photos + photos_inf[num_first_broken_photos].name);
    positions_images[0] = Rect(Point(0, 0), Point(first_img.cols, first_img.rows));

    // Коэффициент для совмещения.
    int max_allow_diff_pixel = 10;
    int block_size = 3;

    for (int i = num_first_broken_photos + 1; i < num_photos; i++) {
        int x = 0;
        Rect first_img_rect = Rect(0, 0, first_img.cols, first_img.rows);

        Mat second_img = imread(path_norm_photos + photos_inf[i].name);

        // Выбираем область интереса на первой фотографии.
        double size_area_of_interest = 0.5;
        int start_y = -20;
        int end_y = 20;
        if (photos_inf[i].latitude - photos_inf[i - 1].latitude < 0) {
            start_y = first_img.rows * ((1 - size_area_of_interest) / 2);
            end_y = first_img.rows / 2;
        }
        else {
            start_y = -first_img.rows / 2;
            end_y = -first_img.rows * ((1 - size_area_of_interest) / 2);
        }

        // Теперь найдем положение второй картинки относительно первой.
        int best_x = -1;
        int best_y = -1;
        double best_rating = -100;

        for (int y = start_y; y < end_y; y += 9) {
            double curr_rating = 0;
            Rect second_img_rect = Rect(x, y, second_img.cols, second_img.rows);

            Rect intersection_rect = first_img_rect & second_img_rect;
            Mat first_img_part = first_img(intersection_rect);
            intersection_rect -= Point(x, y);
            Mat second_img_part = second_img(intersection_rect);

            uchar* p_first;
            uchar* p_second;
            for (int y_comp = 0; y_comp < first_img_part.rows; y_comp += block_size) {
                for (int x_comp = 0; x_comp < first_img_part.cols; x_comp += block_size) {
                    int num_zeros = 1;
                    int curr_block_size_y = block_size;
                    int curr_block_size_x = block_size;
                    if (first_img_part.rows - y_comp < curr_block_size_y) {
                        curr_block_size_y = first_img_part.rows - y_comp;
                    }
                    if (first_img_part.cols - x_comp < curr_block_size_x) {
                        curr_block_size_x = first_img_part.rows - y_comp;
                    }
                    long int sum_b_block_first = 0;
                    long int sum_g_block_first = 0;
                    long int sum_r_block_first = 0;
                    long int sum_b_block_second = 0;
                    long int sum_g_block_second = 0;
                    long int sum_r_block_second = 0;
                    for (int y_block = 0; y_block < curr_block_size_y; y_block++) {
                        p_first = first_img_part.ptr<uchar>(y_comp + y_block);
                        p_second = second_img_part.ptr<uchar>(y_comp + y_block);
                        for (int x_block = 0; x_block < curr_block_size_x; x_block++) {
                            sum_b_block_first += p_first[3 * (x_comp + x_block)];
                            sum_g_block_first += p_first[3 * (x_comp + x_block) + 1];
                            sum_r_block_first += p_first[3 * (x_comp + x_block) + 2];
                            sum_b_block_second += p_second[3 * (x_comp + x_block)];
                            sum_g_block_second += p_second[3 * (x_comp + x_block) + 1];
                            sum_r_block_second += p_second[3 * (x_comp + x_block) + 2];
                            if (p_first[3 * (x_comp + x_block)] == 0 && p_first[3 * (x_comp + x_block) + 1] == 0 &&
                                p_first[3 * (x_comp + x_block) + 2] == 0) {
                                num_zeros += 1;
                            }
                            if (p_second[3 * (x_comp + x_block)] == 0 && p_second[3 * (x_comp + x_block) + 1] == 0 &&
                                p_second[3 * (x_comp + x_block) + 2] == 0) {
                                num_zeros += 1;
                            }
                        }
                    }
                    double diff_b = abs(sum_b_block_first - sum_b_block_second) / (curr_block_size_x * curr_block_size_y);
                    double diff_g = abs(sum_g_block_first - sum_g_block_second) / (curr_block_size_x * curr_block_size_y);
                    double diff_r = abs(sum_r_block_first - sum_r_block_second) / (curr_block_size_x * curr_block_size_y);
                    if (diff_b < max_allow_diff_pixel && diff_g < max_allow_diff_pixel && diff_r < max_allow_diff_pixel) {
                        curr_rating += (double)1 / num_zeros;
                    }
                }
            }
            if (curr_rating > best_rating) {
                best_rating = curr_rating;
                best_x = x;
                best_y = y;
            }
            printf("x = %d, y = %d, current rating = %lf\n", x, y, curr_rating);
        }
        printf("Best position of second (%d) image relatively first (%d): is x = %d, y = %d \n", i + 1, i, best_x, best_y);

        positions_images[i - num_first_broken_photos] = Rect(positions_images[i - num_first_broken_photos - 1].x + best_x,
            positions_images[i - num_first_broken_photos - 1].y + best_y, second_img.cols, second_img.rows);

        first_img = second_img;
    }
}