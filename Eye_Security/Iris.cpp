#include "main.h"

Mat realKernel, imagKernel;
int xo = 0, yo = 0;
int xp = 0, yp = 0, rp = 0, ri = 0;
void histogramStretching(Mat img, Mat& out)
{
	int lowvalue = 255, highvalue = 0;
	int i, j;
	for (i = 0; i < img.rows; i++)
	{
		for (j = 0; j < img.cols; j++)
		{
			if (lowvalue > img.at<uchar>(i, j))
				lowvalue = img.at<uchar>(i, j);
			if (highvalue < img.at<uchar>(i, j))
				highvalue = img.at<uchar>(i, j);
		}
	}
	float mult = 255 / (float)(highvalue - lowvalue);
	for (i = 0; i < img.rows; i++)
	{
		for (j = 0; j < img.cols; j++)
			out.at<uchar>(i, j) = (uchar)(img.at<uchar>(i, j) - lowvalue) * mult;
	}
}
void histogramEqulization(Mat img, Mat& out)
{
	int i, j;
	unsigned int hist[256] = { 0, };
	float cdf[256] = { 0, };
	int pixel, max;

	for (i = 0; i < img.rows; i++)
	{
		for (j = 0; j < img.cols; j++)
		{
			hist[img.at<uchar>(i, j)]++;
		}
	}
	cdf[0] = hist[0];
	for (i = 1; i < 256; i++)
		cdf[i] = cdf[i - 1] + hist[i];
	max = cdf[255];
	for (i = 0; i < img.rows; i++)
	{
		for (j = 0; j < img.cols; j++)
		{
			pixel = cdf[img.at<uchar>(i, j)] / max * 255;
			out.at<uchar>(i, j) = (pixel > 255) ? 255 : pixel;
		}
	}
}
bool FindPupil(Mat &input)
{
	Mat clone, gray;
	input.copyTo(clone);
	int flag = 0;

	cvtColor(~clone, gray, CV_BGR2GRAY);
	threshold(gray, gray, 190, 255, THRESH_BINARY);

	vector<vector<Point> > contours;
	findContours(gray.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//동공 찾기
	drawContours(gray, contours, -1, CV_RGB(255, 255, 255), -1);

	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		Rect rect = boundingRect(contours[i]);
		int radius = (rect.height + rect.width) / 4;
		double rect_condition = abs(1 - ((double)rect.width / (double)rect.height));
		double circle_condition = abs(1 - (area / (CV_PI * pow(radius, 2))));
		if (area >= 800 && rect_condition <= 0.2 && circle_condition <= 0.2)
		{
			flag = 1;
			xp = rect.x;
			yp = rect.y;
			rp = radius;
		}
	}

	if (flag == 1)
	{
		xo = xp + rp;
		yo = yp + rp;
		//cout << " 동공 성공 " << endl;
		return true;
	}
	else
	{
		//cout << " 동공 실패 " << endl;
		return false;
	}

}
bool FindIris(Mat &input)
{
	Mat clone, tmpclone;

	input.copyTo(clone);
	input.copyTo(tmpclone);

	int x1, x2, y1;

	histogramStretching(clone, clone);
	histogramEqulization(clone, clone);

	cvtColor(clone, clone, CV_BGR2GRAY);
	threshold(clone, clone, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

	if ((xo - 4 * rp) > 0 && (xo + 4 * rp) < clone.cols)
	{
		x1 = abs(xo - 4 * rp);
		//cout << x1 << endl;
		x2 = 8 * rp;
	}
	else
	{
		x1 = 0;
		x2 = clone.cols;
	}


	if ((yo - rp / 2) > 0)
	{
		y1 = yo - rp / 2;
	}
	else
	{
		y1 = 0;
	}
	Mat iris(clone, Rect(x1, y1, x2, rp));//오른쪽 아래 부분

	int *rindex, *lindex, rcount = 0, lcount = 0;
	int l_avg = 0, r_avg = 0;

	rindex = new int[iris.rows]();
	lindex = new int[iris.rows]();

	for (int i = 0; i < iris.rows; i++)
	{
		for (int j = 1; j < iris.cols * 0.3; j++)
		{
			if ((iris.at<uchar>(i, j) == 0) && (iris.at<uchar>(i, j - 1) == 255))
			{
				lindex[i] = j;
			}

		}
		for (int j = iris.cols - 1; j > iris.cols * 0.7; j--)
		{
			if ((iris.at<uchar>(i, j) == 0) && (iris.at<uchar>(i, j + 1) == 255))
			{
				rindex[i] = j;
			}
		}
	}

	for (int i = 0; i < iris.rows; i++)
	{
		if (lindex[i] != 0)
		{
			l_avg += lindex[i];
			lcount++;
		}
		if (rindex[i] != 0)
		{
			r_avg += rindex[i];
			rcount++;
		}
	}
	if (lcount != 0)
		l_avg = (double)abs(l_avg / lcount);
	if (rcount != 0)
		r_avg = (double)abs(r_avg / rcount);

	if (l_avg > iris.cols * 0.05)
	{
		if (r_avg < iris.cols * 0.95)
			ri = (double)abs((r_avg - l_avg) / 2);
		else
			ri = (double)abs(iris.cols / 2 - l_avg);
	}
	else
		ri = (double)abs(r_avg - iris.cols / 2);

	delete[] lindex;
	delete[] rindex;

	if (xo + ri < tmpclone.cols && xo - ri > 0 && yo + ri < tmpclone.rows && yo - ri > 0)
	{
		circle(input, Point(xo, yo), rp, CV_RGB(255, 255, 255), 1); // 동공 그리기
		circle(input, Point(xo, yo), ri, CV_RGB(255, 255, 255), 1); // 동공 그리기
		
		Mat out = Mat(60, 360, CV_8U, cv::Scalar(255));
		cvtColor(tmpclone, tmpclone, CV_BGR2GRAY);
		for (int i = 0; i < 360; i++)
		{
			double alpha = 2 * CV_PI * i / 360;
			for (int j = 0; j < 60; j++)
			{
				double r = (double)j / 60;
				int x = (int)((1 - r)*(xo + rp*cos(alpha)) + r*(xo + ri*cos(alpha)));
				int y = (int)((1 - r)*(yo + rp*sin(alpha)) + r*(yo + ri*sin(alpha)));
				out.at<uchar>(j, i) = tmpclone.at<uchar>(y, x);
			}
		}
		out.copyTo(input);
		histogramEqulization(input, input);
		//cout << " 홍채 성공 " << endl;
		return true;
	}
	else
	{
		//cout << " 홍채 실패 " << endl;
		return false;
	}
}

double filterGabor(Mat &src, Mat &kernel)
{
	double sum = 0;
	for (int p = 0; p < src.rows; p++)
	{
		uchar *data = src.ptr<uchar>(p);
		double *k = kernel.ptr<double>(p);
		for (int q = 0; q < src.cols; q++)
		{
			sum += data[q] * k[q] * p;
		}
	}
	return sum;
}
bool create_kernel(int r, int theta, int alpha, int beta, double omiga, int ktype)
{
	int xmin, ymin, xmax, ymax;
	xmax = theta / 2;
	ymax = r / 2;
	xmin = -xmax;
	ymin = -ymax;
	realKernel = cv::Mat(ymax - ymin + 1, xmax - xmin + 1, ktype);
	imagKernel = cv::Mat(ymax - ymin + 1, xmax - xmin + 1, ktype);
	for (int y = ymin; y <= ymax; y++)
	{
		for (int x = xmin; x <= xmax; x++)
		{
			double o = x * CV_PI / 180;
			double c = cos(-omiga*o), s = sin(-omiga*o);
			double er = -((pow(y, 2)) / (pow(alpha, 2)));
			double eo = -((pow(x, 2)) / (pow(beta, 2)));

			double rv = std::exp(er + eo)*c;
			double iv = std::exp(er + eo)*s;
			if (ktype == CV_32F) {
				realKernel.at<float>(ymax - y, xmax - x) = (float)rv;
				imagKernel.at<float>(ymax - y, xmax - x) = (float)iv;
			}
			else {
				realKernel.at<double>(ymax - y, xmax - x) = rv;
				imagKernel.at<double>(ymax - y, xmax - x) = iv;
			}
		}
	}
	return true;
}
void GaborCode(Mat &input, vector<char> &irisCode)
{
	Mat iris(input, Rect(0, 0, 180, 30));
	Mat subM[12], selM;
	int sli = 6;

	for (int i = 0; i < 6; i++)
	{
		subM[i] = Mat(iris, Rect(180 * (double)i / sli, 0, 180 * 1.0 / sli, 30 * 1 / 2));
	}
	for (int i = 0; i < 6; i++)
	{
		subM[i + 6] = Mat(iris, Rect(180 * (double)i / sli, 30 * 1 / 2, 180 * 1.0 / sli, 30 * 1 / 2));
	}
	for (int i = 0; i < 100; i++)
	{
		if (create_kernel(40, 60, 40, 60, 0.1*(i + 10), CV_32F))
		{
			Mat realK = realKernel;
			Mat imagK = imagKernel;

			for (int i = 0; i < 12; i++)
			{
				double real = filterGabor(subM[i], realK);
				double imag = filterGabor(subM[i], imagK);
				if (real >= 0)
				{
					irisCode.push_back('1');
				}
				else
				{
					irisCode.push_back('0');
				}
				if (imag >= 0)
				{
					irisCode.push_back('1');
				}
				else
				{
					irisCode.push_back('0');
				}
			}
		}
	}
	
	Mat codeimg = Mat(80, 600, CV_8U, cv::Scalar(0));
	for (int i = 0; i < 600; i+=2)
	{
	if (irisCode[i/2] == '1')
	{
	for (int j = 0; j < 10; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i+1) = 0;
	}
	}
	else
	{
	for (int j = 0; j < 10; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}

	if (irisCode[i/2+300] == '1')
	{
	for (int j = 10; j < 20; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i + 1) = 0;
	}
	}
	else
	{
	for (int j = 10; j < 20; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}

	if (irisCode[i/2+600] == '1')
	{
	for (int j = 20; j < 30; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i + 1) = 0;
	}
	}
	else
	{
	for (int j = 20; j < 30; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}

	if (irisCode[i/2+900] == '1')
	{
	for (int j = 30; j < 40; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i + 1) = 0;
	}
	}
	else
	{
	for (int j = 30; j < 40; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}

	if (irisCode[i/2 + 1200] == '1')
	{
	for (int j = 40; j <50; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i + 1) = 0;
	}
	}
	else
	{
	for (int j =40; j < 50; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}

	if (irisCode[i/2 + 1500] == '1')
	{
	for (int j = 50; j < 60; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i + 1) = 0;
	}
	}
	else
	{
	for (int j = 50; j < 60; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}

	if (irisCode[i/2 + 1800] == '1')
	{
	for (int j = 60; j <70; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i + 1) = 0;
	}
	}
	else
	{
	for (int j = 60; j < 70; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}

	if (irisCode[i/2 + 2100] == '1')
	{
	for (int j = 70; j < 80; j++)
	{
	codeimg.at<uchar>(j, i) = 0;
	codeimg.at<uchar>(j, i + 1) = 0;
	}
	}
	else
	{
	for (int j = 70; j < 80; j++)
	{
	codeimg.at<uchar>(j, i) = 255;
	codeimg.at<uchar>(j, i + 1) = 255;
	}
	}
	}
	imshow("code", codeimg);
	moveWindow("code", 2560, 310);
}
bool Recognition(vector<char> code1, vector<char> code2)
{
	int count = 0;
	for (int i = 0; i < code1.size(); i++)
	{
		if (code1[i] != code2[i])
			count++;
	}
	double var = (double)count / (double)code1.size();
	//cout << " var = " << var << endl;

	if (var <= 0.1)
	{
		//cout << "인식되었습니다" << endl;
		return true;
	}

	else return false;

}