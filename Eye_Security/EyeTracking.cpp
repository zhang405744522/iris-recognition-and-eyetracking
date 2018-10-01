
#include"EyeTracking.h"

using namespace std;

void EyeTracking::Direction(Point eyeCenter, Point glintCenter)
{
	int glintCenterX = glintCenter.x;
	int glintCenterY = glintCenter.y;
	
	
	int xDiffer = 0, yDiffer = 0;
	int distance = 0;
	xDiffer = glintCenterX - eyeCenter.x;
	yDiffer = glintCenterY - eyeCenter.y;

	distance = sqrt(pow(xDiffer, 2) + pow(yDiffer, 2));

	int realpass = 0;

	if (distance >= 10)
	{
		if (xDiffer > 0 && yDiffer > 0) {

			if (flagCount > 0 && beforePass != 1)
				flagCount = 0;

			if (beforePass == 1 && flagCount > flagNum)
				realpass = 1;

			flagCount++;
			beforePass = 1;
		}


		else if (xDiffer < 0 && yDiffer > 0) {

			if (flagCount > 0 && beforePass != 2)
				flagCount = 0;

			if (beforePass == 2 && flagCount > flagNum)
				realpass = 2;

			flagCount++;
			beforePass = 2;

		}
		else if (xDiffer > 0 && yDiffer < 0) {

			if (flagCount > 0 && beforePass != 3)
				flagCount = 0;

			if (beforePass == 3 && flagCount > flagNum)
				realpass = 3;

			flagCount++;

			beforePass = 3;
		}
		else if (xDiffer < 0 && yDiffer < 0) {

			if (flagCount > 0 && beforePass != 4)
				flagCount = 0;

			if (beforePass == 4 && flagCount > flagNum)
				realpass = 4;

			flagCount++;

			beforePass = 4;
		}



		if (realpass != 0 && Password.size() == 0)
		{
			Password.push_back(realpass);
			cout << " ■ Pattern : " << realpass << endl;
		}
		else if (realpass != 0 && Password.size()>0)
			if (Password.back() != realpass)
			{
				Password.push_back(realpass);
				cout << " ■ Pattern : " << realpass << endl;
			}

	}




}



void EyeTracking::Eyerecognition(Mat inimg)
{
	
	static int xp, yp, rp, xo, yo;
	
	Mat mTracking;	//임시 저장할 영상
	inimg.copyTo(mTracking);
	resize(mTracking, mTracking, Size(640, 480));
	
	Mat clone;
	mTracking.copyTo(clone);
	imshow("원 이미지", clone);
	moveWindow("원 이미지", 1920, 530);
	vector<Vec3f> circles;

	dilate(mTracking, mTracking, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
	dilate(mTracking, mTracking, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));		    //침식

	erode(mTracking, mTracking, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));			//팽창
	erode(mTracking, mTracking, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

	medianBlur(mTracking, mTracking, 1);
	GaussianBlur(mTracking, mTracking, Size(5, 5), 0);				//미디안 필터, 가우시안 필터
															//imshow("블러", inimg);
	threshold(mTracking, mTracking, 60, 255, 0);				//이진화
	bitwise_not(mTracking, mTracking);					//반전영상
	 imshow("이진화", mTracking);
	 moveWindow("이진화", 2560, 530);

	vector<vector<Point> > contours;
	findContours(mTracking.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//동공 찾기
	drawContours(mTracking, contours, -1, CV_RGB(255, 255, 255), -1);
	int flag = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		Rect rect = boundingRect(contours[i]);
		int radius = (rect.height + rect.width) / 4;
		double rect_condition = abs(1 - ((double)rect.width / (double)rect.height));
		double circle_condition = abs(1 - (area / (CV_PI * pow(radius, 2))));
		if (area >= 200 && rect_condition <= 0.15 && circle_condition <= 0.15)
		{
			flag = 1;
			xp = rect.x;
			yp = rect.y;
			rp = radius;
		}
	}


	if (flag == 1 && rp < 80)
	{
		xo = xp + rp;
		yo = yp + rp;
	}
	else return;

	circle(mTracking, Point(xo, yo), 5, 50, 2, 8);
	


	if (xo >= 50 && yo >= 50 && xo < 500 && yo < 500) {
		Mat glint(clone, Rect(Point(xo + 50, yo + 50), Point(xo - 50, yo - 50)));
		imshow("glint", glint);
		moveWindow("glint", 3220, 530);

		/*       전처리        */
		GaussianBlur(glint, glint, Size(3, 3), 0);			//미디안 필터, 가우시안 필터
		threshold(glint, glint, 200, 255, 0);				//이진화
		erode(glint, glint, getStructuringElement(MORPH_ELLIPSE, Size(2, 2)));			//팽창
		dilate(glint, glint, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)));
	
		/*   글린트 중심 찾기  */
		int rowSum = 0, colSum = 0, countPixel = 0;
		for (int i = 0; i < glint.cols; i++) {
			for (int j = 0; j < glint.rows; j++) {

				if (int(glint.at<uchar>(i, j)) == 255) {
					countPixel++;
					rowSum += i;
					colSum += j;

				}
			}
		}

		/* 글린트가 하나라도 있을때 */
		if (countPixel >0) {		
			int glintCenterX, glintCenterY = 0; //글린트의 중점 평균으로 구하기.
			glintCenterX = colSum / countPixel;
			glintCenterY = rowSum / countPixel;
			circle(glint, Point(glintCenterX, glintCenterY), 10, 255, 1, 8);
			imshow("glintCenter", glint);
			moveWindow("glintCenter", 3320, 530);

			Direction(Point(glint.cols / 2, glint.rows / 2), Point(glintCenterX, glintCenterY));
		}
	}

}

string EyeTracking::GetPassword()
{
	string pass;
	for (int i = 0; i < Password.size(); i++){
	pass +=to_string(Password.at(i));
	}
	return pass;
}
