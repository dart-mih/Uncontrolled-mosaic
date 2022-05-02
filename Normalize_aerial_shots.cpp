// Normalize_aerial_shots.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <fstream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

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
OPENCV_FULL camera parameters:
camera_id - уникальный идентификатор среди камер снимавших одну сцену.
model == OPENCV_FULL
width - ширина кадра в пикселях
height - высота кадра в пикселях
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

void getInfoAboutPhotos(std::string photos_info_path, int num_images, PhotoInf* photos_info_struct) {
    std::ifstream photos_info(photos_info_path);
    if (photos_info.is_open()) {
        for (int i = 0; i < num_images; i++) {
            string tmp;
            photos_info >> photos_info_struct[i].name;

            photos_info >> tmp;
            photos_info_struct[i].latitude = stof(tmp);
            photos_info >> tmp;
            photos_info_struct[i].longitude = stof(tmp);
            photos_info >> tmp;
            photos_info_struct[i].altBaro = stof(tmp);

            photos_info >> tmp;
            photos_info_struct[i].roll = stof(tmp);
            photos_info >> tmp;
            photos_info_struct[i].pitch = stof(tmp);
            photos_info >> tmp;
            photos_info_struct[i].yaw = stof(tmp);

            photos_info >> photos_info_struct[i].time;
            photos_info >> tmp;
            photos_info_struct[i].time = photos_info_struct[i].time + " " + tmp;

            photos_info >> tmp;
            photos_info_struct[i].altGPS = stof(tmp);
        }
    }

    photos_info.close();
}

void printInfoAboutCamera(CameraInf& photos_info_struct) {
    printf("------------------------------------------------\n");
    printf("------------------------------------------------\n");
    printf("Camera_id: %d \nCamera_model: %s \nPhotos_width: %f \nPhotos_height: %f \nfx: %f \nfy: %f \nCenter_x: %f \nCenter_y: %f \n",
        photos_info_struct.camera_id, photos_info_struct.model.c_str(), photos_info_struct.width,
        photos_info_struct.height, photos_info_struct.fx, photos_info_struct.fy, 
        photos_info_struct.center_x, photos_info_struct.center_y);
    printf("------------------------------------------------\n");
    printf("Other_correcting_parameters: \nk1: %f \nk2: %f \np1: %f \np2: %f \nk3: %f \nk4: %f \nk5: %f \nk6: %f \n",
        photos_info_struct.k1, photos_info_struct.k2, photos_info_struct.p1, 
        photos_info_struct.p2, photos_info_struct.k3, photos_info_struct.k4,
        photos_info_struct.k5, photos_info_struct.k6);
    printf("------------------------------------------------\n");
    printf("------------------------------------------------\n");
}

void getInfoAboutCamera(std::string camera_info_path, CameraInf& camera_info_struct) {
    std::ifstream camera_info(camera_info_path);
    if (camera_info.is_open()) {
        string tmp;

        camera_info >> tmp;
        camera_info_struct.camera_id = stoi(tmp);
        camera_info >> camera_info_struct.model;
        camera_info >> tmp;
        camera_info_struct.width = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.height = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.fx = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.fy = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.center_x = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.center_y = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.k1 = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.k2 = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.p1 = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.p2 = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.k3 = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.k4 = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.k5 = stof(tmp);
        camera_info >> tmp;
        camera_info_struct.k6 = stof(tmp);
    }

    camera_info.close();
}

void printInfoAboutPhotos(int num_images, PhotoInf* photos_info_struct) {
    for (int i = 0; i < num_images; i++) {
        printf("Photo_name: %s \nLatitude: %f \nLongitude: %f \nAltBaro: %f \nRoll: %f \nPitch: %f \nYaw: %f \nTime: %s \nAltGPS: %f \n", 
            photos_info_struct[i].name.c_str(), photos_info_struct[i].latitude,
            photos_info_struct[i].longitude, photos_info_struct[i].altBaro,
            photos_info_struct[i].roll, photos_info_struct[i].pitch,
            photos_info_struct[i].yaw, photos_info_struct[i].time.c_str(),
            photos_info_struct[i].altGPS);
        printf("------------------------------------------------\n");
    }
}

/*
image - исходная картинка
result - результат
(Повороты происходят относительно центра изображения)
alpha - угол поворота относительно Ox (в градусах).
beta - угол поворота относительно Oy (в градусах).
gamma - угол поворота относительно Oz (в градусах).
point_rotate_x - координата точки, относительно которой происходит поворот картинки
point_rotate_y - координата точки, относительно которой происходит поворот картинки
f - Расстояние до камеры (в пикселях изображения).
*/
void rotateImage(const Mat& image, Mat& result, double alpha, double beta, double gamma, 
    double point_rotate_x, double point_rotate_y, double f) {
    alpha = alpha * CV_PI / 180.;
    beta = beta * CV_PI / 180.;
    gamma = gamma * CV_PI / 180.;

    // Проекция 2D -> 3D матриц.
    Mat A1 = (Mat_<double>(4, 3) <<
        1, 0, -point_rotate_x,
        0, 1, -point_rotate_y,
        0, 0, 0,
        0, 0, 1);

    // Матрицы поворота относительно осей X, Y, и Z.
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

    // Общая матрица всех поворотов (RX, RY, RZ).
    Mat R = RX * RY * RZ;

    // Матрица смещения 
    Mat T = (Mat_<double>(4, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, f,
        0, 0, 0, 1);

    // Обратная проекция матриц 3D -> 2D.
    Mat A2 = (Mat_<double>(3, 4) <<
        f, 0, point_rotate_x, 0,
        0, f, point_rotate_y, 0,
        0, 0, 1, 0);

    // Матрица результирующей трансформации.
    Mat final_trans_mat = A2 * (T * (R * A1));

    // Применяем матричную трансформацию
    warpPerspective(image, result, final_trans_mat, image.size(), INTER_LANCZOS4);
}

int main() {
    int num_photos = 40;
    PhotoInf* photos_inf = new PhotoInf[num_photos];
    CameraInf camera_inf;
    string src = "src/";
    string output = "result/";

    getInfoAboutCamera("src/cameras.txt", camera_inf);
    getInfoAboutPhotos("src/telemetry.txt", num_photos, photos_inf);
    printInfoAboutPhotos(num_photos, photos_inf);
    printInfoAboutCamera(camera_inf);

    // Выбираем фотографию, относительно показателей которой мы будем поворачивать остальные изображения;

    PhotoInf correcting_info = photos_inf[2];

    double pix_in_one_meter =  (camera_inf.width * 0.4) / (abs(photos_inf[2].longitude - photos_inf[3].longitude) * 111134.86); // Примерно пикселей в одном метре.
    double distance = photos_inf[2].altBaro * pix_in_one_meter; // Примерная дистанция до камеры.
    printf("Distance: %lf\n", distance);

    for (int i = 2; i < num_photos; i++) {
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

    delete[] photos_inf;
    return 0;
}
