#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

#include "GetInfoFromOutput.h"
#include "../Shots_normalization/PhotoAndCameraInf.h"
#include "../Shots_normalization/PhotoAndCameraInfGetFunc.h"

using namespace std;
using namespace cv;

/*
A structure containing performance estimates for the overlay algorithm.
average_process_time_per_photo - The average processing time for overlaying two photos (ms).
time_to_all_photos - time to process all photos in the set (ms).
average_difference_of_pixels_overlaid_photos - average pixel difference at intersections of overlaid photos.
average_num_important_points_intersect - number of coincident positions of selected important points in two pictures.
*/
struct OverlayAlgorithmEstimates {
	double average_process_time_per_photo = 0;
	double time_to_all_photos = 0;
	double average_difference_of_pixels_overlaid_photos = 0;
	double average_num_important_points_intersect = 0;
};

/*
A structure containing performance estimates for the stitching algorithm.
average_process_time_per_photo - The average processing time for writing a photo on the canvas (ms).
time_to_all_photos - time to process all photos in the set (ms).
*/
struct StitchAlgorithmEstimates {
	double average_process_time_per_photo = 0;
	double time_to_all_photos = 0;
};

/*
A function that calculates the efficiency scores of the photo overlay and stitching algorithm.
results_overlay_alg - structure that will contain the estimates of the overlay algorithm.
results_stitch_alg - structure that will contain the stitching algorithm scores.
path_to_alg_output - path to a file containing information about the operation of the overlay and stitching algorithms.
path_to_photos - the path to the folder containing the photos that were overlaid.
path_to_photos_info - path to a file containing information about each photo.
num_photos - the number of photos processed by the algorithm.
*/
void algorithmPerfomarmanceEvaluation(OverlayAlgorithmEstimates& results_overlay_alg, StitchAlgorithmEstimates& results_stitch_alg,
	std::string path_to_alg_output, std::string path_to_photos, std::string path_to_photos_info, int num_photos) {
	ifstream overlay_res_info(path_to_alg_output);

	vector<CmpPhotos> cmp_vec;
	vector<StitchPhotos> stitch_vec;

	parseCmp(overlay_res_info, cmp_vec);
	parseStitch(overlay_res_info, stitch_vec);

	PhotoInf* photos_inf = new PhotoInf[num_photos];
	getInfoAboutPhotos(path_to_photos_info, num_photos, photos_inf);

	// Calculate the total and average time for the overlay algorithm.
	for (int i = 0; i < cmp_vec.size(); i++) {
		results_overlay_alg.time_to_all_photos += cmp_vec[i].time;
	}
	results_overlay_alg.average_process_time_per_photo = results_overlay_alg.time_to_all_photos / cmp_vec.size();

	// Calculate the total and average time for the stitching algorithm.
	for (int i = 0; i < stitch_vec.size(); i++) {
		results_stitch_alg.time_to_all_photos += stitch_vec[i].time;
	}
	results_stitch_alg.average_process_time_per_photo = results_stitch_alg.time_to_all_photos / stitch_vec.size();

	// Calculate average pixel difference at intersections of overlaid photos and average estimated num content intersection points.
	double pixel_diff_all_intersection = 0;
	int num_intersections = cmp_vec.size();

	double average_num_important_points_intersect = 0;

	Mat first_img;
	Mat second_img;
	for (int i = 0; i < cmp_vec.size(); i++) {
		first_img = imread(path_to_photos + photos_inf[cmp_vec[i].num_photo1 - 1].name);
		second_img = imread(path_to_photos + photos_inf[cmp_vec[i].num_photo2 - 1].name);
		Point relative_pos = Point(cmp_vec[i].dx, cmp_vec[i].dy);

		Rect first_img_rect = Rect(0, 0, first_img.cols, first_img.rows);
        Rect second_img_rect = Rect(relative_pos.x, relative_pos.y, second_img.cols, second_img.rows);

        Rect intersection_rect = first_img_rect & second_img_rect;
        Mat first_img_part = first_img(intersection_rect);
        intersection_rect -= Point(relative_pos.x, relative_pos.y);
        Mat second_img_part = second_img(intersection_rect);

		if (intersection_rect.area() == 0) {
			num_intersections--;
		}

		int num_channels = first_img.channels();

		// Calculate average pixel difference at intersections of overlaid photos
		double pixel_diff_one_intersection = 0;
		double pixel_diff;

        uchar* p_first;
        uchar* p_second;
		for (int y = 0; y < first_img_part.rows; y += 1) {
			p_first = first_img_part.ptr<uchar>(y);
			p_second = second_img_part.ptr<uchar>(y);
			for (int x = 0; x < first_img_part.cols; x += 1) {
				pixel_diff = 0;
				for (int channel = 0; channel < num_channels; channel++) {
					pixel_diff += (double)(p_first[num_channels * x + channel] - p_second[num_channels * x + channel]) *
						(p_first[num_channels * x + channel] - p_second[num_channels * x + channel]);
				}
				pixel_diff_one_intersection += sqrt(pixel_diff) / num_channels;
			}
		}
		pixel_diff_one_intersection /= (first_img_part.rows * first_img_part.cols);
		pixel_diff_all_intersection += pixel_diff_one_intersection;

		// Calculate approximate difference in the content of the intersections.
		Mat first_img_part_areas_of_interest;
		cvtColor(first_img_part, first_img_part_areas_of_interest, COLOR_BGR2GRAY);
		cornerHarris(first_img_part_areas_of_interest, first_img_part_areas_of_interest, 2, 3, 0.01);
		threshold(first_img_part_areas_of_interest, first_img_part_areas_of_interest, 0.00001, 255, THRESH_BINARY);
		Mat second_img_part_areas_of_interest;
		cvtColor(second_img_part, second_img_part_areas_of_interest, COLOR_BGR2GRAY);
		cornerHarris(second_img_part_areas_of_interest, second_img_part_areas_of_interest, 2, 3, 0.01);
		threshold(second_img_part_areas_of_interest, second_img_part_areas_of_interest, 0.00001, 255, THRESH_BINARY);

		for (int y = 0; y < first_img_part.rows; y += 1) {
			p_first = first_img_part_areas_of_interest.ptr<uchar>(y);
			p_second = second_img_part_areas_of_interest.ptr<uchar>(y);
			for (int x = 0; x < first_img_part.cols; x += 1) {
				if (p_second[x] > 0 && p_first[x] > 0) {
					average_num_important_points_intersect++;
				}
			}
		}
	}
	pixel_diff_all_intersection /= num_intersections;
	results_overlay_alg.average_difference_of_pixels_overlaid_photos = pixel_diff_all_intersection;

	average_num_important_points_intersect /= num_intersections;
	results_overlay_alg.average_num_important_points_intersect = average_num_important_points_intersect;
}

int main() {
	// With the same parameters, the algorithms should have been run.
	
	int num_photos = 40;
	int num_first_broken_photos = 2;

	// Write info in file or console.
	bool write_in_file = 1;
	string path_to_file_to_write_info = "Performance_res_2.txt";
	ofstream file_stream;
	if (write_in_file) {
		file_stream.open(path_to_file_to_write_info);
		cout.set_rdbuf(file_stream.rdbuf());
	}

	string path_to_alg_output = "../Results_output/output42.txt";
	string path_to_photos = "../Shots_normalization/result/";
	string path_to_photos_info = "../Shots_normalization/src/telemetry.txt";

	OverlayAlgorithmEstimates results_overlay_alg;
	StitchAlgorithmEstimates results_stitch_alg;

	algorithmPerfomarmanceEvaluation(results_overlay_alg, results_stitch_alg, path_to_alg_output, path_to_photos, path_to_photos_info,
		num_photos);

	cout << "Overlay algorithm performance results:\n";
	cout << "Average time per overlay pair of photos: " << results_overlay_alg .average_process_time_per_photo << "\n";
	cout << "Total photo overlay time: " << results_overlay_alg.time_to_all_photos << "\n";
	cout << "Average pixel difference between photo overlay areas: " << results_overlay_alg.average_difference_of_pixels_overlaid_photos << "\n";
	cout << "Rough estimate of the content match in the intersection zones: " << results_overlay_alg.average_num_important_points_intersect << "\n";

	cout << "--------------------------------------------------------\n";

	cout << "Stitch algorithm performance results:\n";
	cout << "Average processing time for writing a photo on the canvas: " << results_overlay_alg.average_process_time_per_photo << "\n";
	cout << "Total photo stitch time: " << results_overlay_alg.time_to_all_photos << "\n";

	if (write_in_file) {
		file_stream.close();
	}
	
	return 0;
}