#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

/*
A structure that stores information about the time and result of overlaying two photos.
numPhoto1 - number of the first photo among overlay photos.
numPhoto2 - number of the second photo among overlay photos.
dx - offset of the second photo relative to the first one by Ox.
dy - offset of the second photo relative to the first one by Oy.
time - time to overlay two photos.
*/
struct CmpPhotos {
	int num_photo1;
	int num_photo2;
	int dx;
	int dy;
	int time;
};

/*
numPhoto - number of the photo among overlay photos.
time - time to stitch two photos.
*/
struct StitchPhotos {
	int num_photo;
	int time;
};

/*
A function that reads information about the operation of the photo overlay algorithm.
overlay_res_info - open stream to a file containing the results of the overlay algorithm.
cmp_vec - a vector of structures that will contain information about the overlay of each photo by the algorithm.
*/
void parseCmp(ifstream& overlay_res_info, vector<CmpPhotos>& cmp_vec) {
	string buf;
	overlay_res_info >> buf;
	string str;
	
	while (buf == "Relative") {
		CmpPhotos tmp;

		getline(overlay_res_info, str);
		string str1 = str.substr(7, 12);
		string str2 = str.substr(27, 36);
		tmp.num_photo2 = atoi(str1.c_str());
		tmp.num_photo1 = atoi(str2.c_str());

		getline(overlay_res_info, str);
		str1 = str.substr(str.find("x") + 2, str.find("x") + 7);
		tmp.dx = atoi(str1.c_str());
		str1 = str.substr(str.find("y")+2, str.find("y") + 7);
		tmp.dy = atoi(str1.c_str());

		getline(overlay_res_info, str);
		str1 = str.substr(str.find(":") + 2, str.find("x") + 10);
		tmp.time = atoi(str1.c_str());
		cmp_vec.push_back(tmp);

		overlay_res_info >> buf;
	};
}

/*
A function that reads information about the operation of the photo stitching algorithm.
overlay_res_info - open stream to a file containing the results of the stitching algorithm.
cmp_vec - a vector of structures that will contain information about the time each photo was written to the canvas.
*/
void parseStitch(ifstream& overlay_res_info, vector<StitchPhotos>& stitch_vec) {
	string buf = "Time";
	string str;

	do {
		StitchPhotos tmp;

		getline(overlay_res_info, str);
		string str1 = str.substr(10, 20);
		tmp.num_photo = atoi(str1.c_str());

		getline(overlay_res_info, str);
		str1 = str.substr(str.find(">") + 2, str.find(">") + 6);
		tmp.time = atoi(str1.c_str());
		stitch_vec.push_back(tmp);

	} while (overlay_res_info >> buf);
}