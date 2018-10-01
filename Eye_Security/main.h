#include <iostream>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <opencv\cv.h>
#include <opencv2/opencv.hpp>
#include <queue>
#include <string>
#include <stdlib.h>
#include "SerialPort.h"
#include <Windows.h>
#include<vector>

#define _CRT_SECURE_NO_WARNING

using namespace std;
using namespace cv;

#pragma warning(disable:4996)
#pragma once
void histogramStretching(Mat img, Mat& out);
void histogramEqulization(Mat img, Mat& out);

extern int MAX_USER;

bool Data_load(void);
bool Data_save(void);

struct UserData {
	Mat iris[20];
	int pattern = 0;
	string name;
	int num = 0;
	int recognition_count = 0;
	vector< vector<char> > irisCode;
};

bool FindPupil(Mat &input);
bool FindIris(Mat &input);
void GaborCode(Mat &input, vector<char> &irisCode);
double filterGabor(cv::Mat &src, cv::Mat &kernel);
bool Recognition(vector<char> code1, vector<char> code2);
