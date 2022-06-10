#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>

#include "Shots_normalization/NormalizationFunction.h"
#include "Shots_normalization/PhotoAndCameraInf.h"
#include "Shots_normalization/PhotoAndCameraInfGetFunc.h"

#include "Overlay_algorithms/JustGPSalg.h"
#include "Overlay_algorithms/PixelCompareAlg.h"
#include "Overlay_algorithms/ColorChangeCompareAlg.h"
#include "Overlay_algorithms/CompareAndGPS.h"

#include "CombinePhotosAlgs.h"

using namespace std;
using namespace cv;

Mat maskBlackPixels(Mat& img) {
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    Mat mask;
    inRange(hsv, Vec3i{0, 0, 0}, Vec3i{179, 255, 3}, mask);
    bitwise_not(mask, mask);
    return mask;
}

void equalizeBrightness(Mat& img) {
    Mat hsv;
    cvtColor(img, hsv, COLOR_BGR2HSV);

    vector<Mat> channels;
    split(hsv, channels);

    // Region histogram equalization.
    auto clahe = createCLAHE(2.0, Size(8, 8));
    clahe->apply(channels[1], channels[1]);
    clahe->apply(channels[2], channels[2]);

    merge(channels, hsv);
    cvtColor(hsv, img, COLOR_HSV2BGR);
}

int main() {
    // Parameters that select the algorithm for overlaying images, combining them, and whether to normalize images at all.
    int choosen_alg = 2;
    int choosen_combine_photos_func = 1;
    double resize_ratio = 0.5;

    bool normalize_images = 0;
    bool equalize_brightness = 0;
    bool fix_angles = 1;

    // Write info about algotithms in file or console.
    bool write_in_file = 0;
    string path_to_file_to_write_info = "output.txt";

    ofstream file_stream;

    if (write_in_file) {
        file_stream.open(path_to_file_to_write_info);
        cout.set_rdbuf(file_stream.rdbuf());
    }

    int num_photos = 40;
    int num_first_broken_photos = 4; // The number of first incorrect photos in the set.
    int num_channels = 3;

    string path_src_photos = "Shots_normalization/src/";
    string path_norm_photos = "Shots_normalization/result/";
    string res_img_path = "Result_overlay.jpg";

    PhotoInf* photos_inf = new PhotoInf[num_photos];
    CameraInf camera_inf;
    
    // Get information about images and the camera.
    getInfoAboutCamera(path_src_photos + "cameras.txt", camera_inf);
    getInfoAboutPhotos(path_src_photos + "telemetry.txt", num_photos, photos_inf);
    printInfoAboutPhotos(num_photos, photos_inf);
    printInfoAboutCamera(camera_inf);

    if (fix_angles) {
        for (int i = num_first_broken_photos; i < num_photos; i++) {
            photos_inf[i].roll = -1.2;
            photos_inf[i].pitch = -2.4;
        }
    }

    if (normalize_images) {
        // Now normalize the images.
        Mat image;
        Mat result;
        Mat prev_img;
        for (int i = num_first_broken_photos; i < num_photos; i++) {
            image = imread(path_src_photos + photos_inf[i].name);
            if (equalize_brightness) {
                equalizeBrightness(image);
            }
            normalizeShot(image, result, photos_inf[i], camera_inf);
            printf("%s rotated.\n", photos_inf[i].name.c_str());
            imwrite(path_norm_photos + photos_inf[i].name, result);
        }
    }

    // Will contain information about the position of each image on the general canvas.
    Rect* positions_images = new Rect[num_photos - num_first_broken_photos];

    // Finding the position of the images.
    Mat first_img = imread(path_norm_photos + photos_inf[num_first_broken_photos].name);
    positions_images[0] = Rect(Point(0, 0), Point(first_img.cols, first_img.rows));

    if (resize_ratio < 1) {
        resize(first_img, first_img, Size(first_img.cols * resize_ratio, first_img.rows * resize_ratio));
    }

    for (int i = num_first_broken_photos + 1; i < num_photos; i++) {
        unsigned int start_time = clock();
        Mat second_img = imread(path_norm_photos + photos_inf[i].name);

        int width = second_img.cols;
        int height = second_img.rows;
        if (resize_ratio < 1) {
            resize(second_img, second_img, Size(second_img.cols * resize_ratio, second_img.rows * resize_ratio));
        }

        Point relative_pos;

        // Get black pixel masks.
        Mat mask_first = maskBlackPixels(first_img);
        Mat mask_second = maskBlackPixels(second_img);

        if (choosen_alg == 1) {
            relative_pos = justGPSalg(first_img.rows * (1. / resize_ratio), second_img.rows * (1. / resize_ratio),
                photos_inf[i - 1], photos_inf[i], camera_inf);
        } else if ((choosen_alg == 2) || (choosen_alg == 3)) {
            bool one_direction = 0;
            // Choice of area of interest y.
            int vertical_shift = (3. / 40) * first_img.rows;
            int horizontal_shift = second_img.cols * (1. / 12);

            int cent_y = (13. / 40) * first_img.rows;

            if (photos_inf[i].latitude - photos_inf[i - 1].latitude < 0) {
                cent_y = -(13. / 40) * first_img.rows;
                if (!one_direction) {
                    horizontal_shift = first_img.cols * (1. / 4);
                }
                one_direction = 1;
            } else {
                if (one_direction) {
                    horizontal_shift = first_img.cols * (1. / 4);
                }
                one_direction = 0;
            }

            Point center_search_pos = Point(0, cent_y);

            if (choosen_alg == 2) {
                relative_pos = pixelCompareAlg(first_img, second_img, mask_first, mask_second, photos_inf[i - 1], photos_inf[i], vertical_shift,
                    horizontal_shift, center_search_pos);
            } else {
                relative_pos = hsvCompareAlg(first_img, second_img, mask_first, mask_second, photos_inf[i - 1], photos_inf[i], vertical_shift,
                    horizontal_shift, center_search_pos);
            }
        } else if (choosen_alg == 4) {
            int vertical_shift = first_img.rows * (1. / 16);
            int horizontal_shift = first_img.cols * (1. / 24);

            relative_pos = compareAndGPSalg(first_img, second_img, mask_first, mask_second, 
                photos_inf[i - 1], photos_inf[i], camera_inf, resize_ratio, vertical_shift, horizontal_shift);
        }

        unsigned int end_time = clock();

        // Fill the array of positions.
        if ((resize_ratio < 1) && (choosen_alg != 1)) {
            relative_pos.x *= (1. / resize_ratio);
            relative_pos.y *= (1. / resize_ratio);
        }
        positions_images[i - num_first_broken_photos] = Rect(positions_images[i - num_first_broken_photos - 1].x + relative_pos.x,
            positions_images[i - num_first_broken_photos - 1].y + relative_pos.y, width, height);

        cout << "Relative pos of " << i + 1 << " image relatively " << i 
            <<" image:\n--> x=" << relative_pos.x << ", y=" << relative_pos.y << "\n";
        cout << "--> Time to find pos: " << end_time - start_time << " ms.\n";
        first_img = second_img;
    }

    // Combine images according to the found positions.
    if (choosen_combine_photos_func == 1) {
        combinePhotos(num_photos, num_first_broken_photos, photos_inf, path_norm_photos, positions_images, res_img_path);
    } else if (choosen_combine_photos_func == 2) {
        combinePhotosOptimized(num_photos, num_first_broken_photos, photos_inf, path_norm_photos, positions_images, res_img_path);
    }
    delete[] positions_images;
    delete[] photos_inf;
    if (write_in_file) {
        file_stream.close();
    }
    return 0;
}
