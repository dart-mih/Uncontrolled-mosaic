#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

struct CmpPhotos
{
	int NumPhoto1;
	int NumPhoto2;
	int dx;
	int dy;
	int time;
};

struct StitchPhotos
{
	int NumPhoto;
	int time;
};

void ParseCmp(ifstream& photo_info, vector<CmpPhotos> &Cmp_vec)
{
	string buf;
	photo_info >> buf;
	string str;
	
	while (buf == "Relative")
	{
		CmpPhotos tmp;

		getline(photo_info, str);
		string str1 = str.substr(7, 12);
		string str2 = str.substr(27, 36);
		tmp.NumPhoto1 = atoi(str1.c_str());
		tmp.NumPhoto2 = atoi(str2.c_str());

		getline(photo_info, str);
		str1 = str.substr(str.find("x") + 2, str.find("x") + 6);
		tmp.dx = atoi(str1.c_str());
		str1 = str.substr(str.find("y")+2, str.find("y") + 6);
		tmp.dy = atoi(str1.c_str());

		getline(photo_info, str);
		str1 = str.substr(str.find(":") + 2, str.find("x") + 10);
		tmp.time = atoi(str1.c_str());
		Cmp_vec.push_back(tmp);

		photo_info >> buf;
	};
}

void ParseStitch(ifstream& photo_info, vector<StitchPhotos>& Stitch_vec)
{
	string buf = "Time";
	string str;

	do 
	{
		StitchPhotos tmp;

		getline(photo_info, str);
		string str1 = str.substr(10, 20);
		tmp.NumPhoto = atoi(str1.c_str());

		getline(photo_info, str);
		str1 = str.substr(str.find(">") + 2, str.find(">") + 6);
		tmp.time = atoi(str1.c_str());
		Stitch_vec.push_back(tmp);

	} while (photo_info >> buf);
}

int main()
{
	string path = "output.txt";
	ifstream photo_info(path);
	vector<CmpPhotos> Cmp_vec;
	vector<StitchPhotos> Stitch_vec;
	ParseCmp(photo_info, Cmp_vec);
	ParseStitch(photo_info, Stitch_vec);
	/*for (int i = 0; i < Cmp_vec.size(); i++)
		cout << Cmp_vec[i].NumPhoto2 << endl;
	for (int i = 0; i < Stitch_vec.size(); i++)
		cout << Stitch_vec[i].NumPhoto << endl;*/

	return 0;
}