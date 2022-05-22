#include <iostream>
#include <opencv2/opencv.hpp>

#include "Shots_normalization/Normalization_function.h"
#include "Shots_normalization/Photo_and_camera_inf.h"
#include "Shots_normalization/Photo_and_camera_inf_get_func.h"

#include "Overlay_algorithms/JustGPSalg.h"

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
    // Найдем размер полотна.
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

    // Сместим все картинки.
    for (int i = 0; i < num_photos - num_first_broken_photos; i++) {
        find_positions_images[i] -= res_img_left_top;
    }

    // Теперь найдем размер полотна и создадим его.
    int res_width = res_img_right_down.x - res_img_left_top.x;
    int res_height = res_img_right_down.y - res_img_left_top.y;

    Mat one_img = imread(path_norm_photos + photos_inf[num_first_broken_photos].name);
    Mat result = Mat(res_height, res_width, one_img.type(), Scalar(0, 0, 0));

    // Заполним полотно изображениями.
    uchar* p_img;
    uchar* p_res;
    for (int i = 0; i < num_photos - num_first_broken_photos; i++) {
        Mat img = imread(path_norm_photos + photos_inf[i + num_first_broken_photos].name);
        for (int y = 0; y < img.rows; y++) {
            p_img = img.ptr<uchar>(y);
            p_res = result.ptr<uchar>(y + find_positions_images[i].y);
            for (int x = 0; x < img.cols; x++) {
                if (!(p_img[3 * x] == 0 && p_img[3 * x + 1] == 0 && p_img[3 * x + 2] == 0)) {
                    p_res[3 * (find_positions_images[i].x + x)] = p_img[3 * x];
                    p_res[3 * (find_positions_images[i].x + x) + 1] = p_img[3 * x + 1];
                    p_res[3 * (find_positions_images[i].x + x) + 2] = p_img[3 * x + 2];
                }
            }
        }
    }

    imwrite(res_img_path, result);
}

int main() {
    int num_photos = 40;
    int num_first_broken_photos = 2; // The number of first incorrect photos in the set.
    int num_channels = 3;

    string path_src_photos = "Shots_normalization/src/";
    string  path_norm_photos = "Shots_normalization/result/";
    string res_img_path = "Result_overlay.jpg";

    PhotoInf* photos_inf = new PhotoInf[num_photos];
    CameraInf camera_inf;
    
    // Get information about images and the camera.
    getInfoAboutCamera(path_src_photos + "cameras.txt", camera_inf);
    getInfoAboutPhotos(path_src_photos + "telemetry.txt", num_photos, photos_inf);
    printInfoAboutPhotos(num_photos, photos_inf);
    printInfoAboutCamera(camera_inf);

    // Now normalize the image.
    normalizeShots(path_src_photos, path_norm_photos, photos_inf, camera_inf, num_photos, num_first_broken_photos);

    // Will contain information about the position of each image on the general canvas.
    Rect* positions_images = new Rect[num_photos - num_first_broken_photos];

    // Finding the position of the images.
    justGPSalg(num_photos, num_first_broken_photos, photos_inf, camera_inf, path_norm_photos, positions_images);

    // Combine images according to the found positions.
    combinePhotos(num_photos, num_first_broken_photos, photos_inf, path_norm_photos, positions_images, 
        res_img_path);

    delete[] positions_images;
    delete[] photos_inf;
    return 0;
}
