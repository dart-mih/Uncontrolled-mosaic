#pragma once

#include <string>
#include <fstream>

#include "Photo_and_camera_inf.h"

using namespace std;

/*
Reads all the information for each photo from a file.
photos_info_path - path to a file containing information about photos.
num_images - number of photos.
photos_info_struct - an array of PhotoInf structures, for which memory has already been allocated in advance.
*/
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

/*
Prints all the information for each photo read.
num_images - number of photos read.
photos_info_struct - an array of PhotoInf structures containing information about frames.
*/
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
Reads all camera information from a file.
camera_info_path - path to the camera information file.
camera_info_struct - a structure that will contain the read information about the camera.
*/
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

/*
Prints all the camera information read.
camera_info_struct - a structure containing the read information about the camera.
*/
void printInfoAboutCamera(CameraInf& camera_info_struct) {
    printf("------------------------------------------------\n");
    printf("------------------------------------------------\n");
    printf("Camera_id: %d \nCamera_model: %s \nPhotos_width: %f \nPhotos_height: %f \nfx: %f \nfy: %f \nCenter_x: %f \nCenter_y: %f \n",
        camera_info_struct.camera_id, camera_info_struct.model.c_str(), camera_info_struct.width,
        camera_info_struct.height, camera_info_struct.fx, camera_info_struct.fy,
        camera_info_struct.center_x, camera_info_struct.center_y);
    printf("------------------------------------------------\n");
    printf("Other_correcting_parameters: \nk1: %f \nk2: %f \np1: %f \np2: %f \nk3: %f \nk4: %f \nk5: %f \nk6: %f \n",
        camera_info_struct.k1, camera_info_struct.k2, camera_info_struct.p1,
        camera_info_struct.p2, camera_info_struct.k3, camera_info_struct.k4,
        camera_info_struct.k5, camera_info_struct.k6);
    printf("------------------------------------------------\n");
    printf("------------------------------------------------\n");
}