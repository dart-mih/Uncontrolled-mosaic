#include <iostream>
#include <ctime>
#include <opencv2/opencv.hpp>

#include "Shots_normalization/Normalization_function.h"
#include "Shots_normalization/PhotoAndCameraInf.h"
#include "Shots_normalization/PhotoAndCameraInfGetFunc.h"

#include "Overlay_algorithms/JustGPSalg.h"
#include "Overlay_algorithms/PixelCompareAlg.h"

#include "combinePhotosAlgs.h"

using namespace std;
using namespace cv;

int main() {
    // Parameters that select the algorithm for overlaying images, combining them, and whether to normalize images at all.
    int choosen_alg = 1;
    int choosen_combine_photos_func = 1;
    bool normalize_images = 0;
    // Write info about algotithms in file or console.
    bool write_in_file = 0;
    string path_to_file_to_write_info = "output.txt";

    ofstream file_stream;

    if (write_in_file) {
        file_stream.open(path_to_file_to_write_info);
        cout.set_rdbuf(file_stream.rdbuf());
    }

    int num_photos = 40;
    int num_first_broken_photos = 2; // The number of first incorrect photos in the set.
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

    double norm_distance = getNormalizationDistance(photos_inf[num_first_broken_photos], photos_inf[num_first_broken_photos + 1],
        camera_inf.height);

    if (normalize_images) {
        // Now normalize the images.
        for (int i = num_first_broken_photos; i < num_photos; i++) {
            normalizeShot(path_src_photos, path_norm_photos, photos_inf[i], camera_inf, norm_distance,
                photos_inf[num_first_broken_photos].altBaro);
        }
    }

    // Will contain information about the position of each image on the general canvas.
    Rect* positions_images = new Rect[num_photos - num_first_broken_photos];

    // Finding the position of the images.
    Mat first_img = imread(path_norm_photos + photos_inf[num_first_broken_photos].name);
    positions_images[0] = Rect(Point(0, 0), Point(first_img.cols, first_img.rows));

    for (int i = num_first_broken_photos + 1; i < num_photos; i++) {
        unsigned int start_time = clock();
        Mat second_img = imread(path_norm_photos + photos_inf[i].name);

        Point relative_pos;
        if (choosen_alg == 1) {
            relative_pos = justGPSalg(first_img, second_img, photos_inf[i - 1], photos_inf[i], camera_inf, norm_distance);
        } else {
            relative_pos = pixelCompareAlg(first_img, second_img, photos_inf[i - 1], photos_inf[i]);
        }
        unsigned int end_time = clock();


        // Fill the array of positions.
        positions_images[i - num_first_broken_photos] = Rect(positions_images[i - num_first_broken_photos - 1].x + relative_pos.x,
            positions_images[i - num_first_broken_photos - 1].y + relative_pos.y, second_img.cols, second_img.rows);

        cout << "Relative pos of " << i + 1 << " image relatively " << i 
            <<" image:\n--> x=" << relative_pos.x << ", y=" << relative_pos.y << "\n";
        cout << "--> Time to find pos: " << end_time - start_time << " ms.\n";
        first_img = second_img;
    }

    // Combine images according to the found positions.
    if (choosen_combine_photos_func == 1) {
        combinePhotos(num_photos, num_first_broken_photos, photos_inf, path_norm_photos, positions_images, res_img_path);
    } else {
        combinePhotosOptimized(num_photos, num_first_broken_photos, photos_inf, path_norm_photos, positions_images, res_img_path);
    }

    delete[] positions_images;
    delete[] photos_inf;
    if (write_in_file) {
        file_stream.close();
    }
    return 0;
}
