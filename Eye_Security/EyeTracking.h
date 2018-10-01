#pragma once
#include"main.h"

class EyeTracking {
public:
	void Direction(Point eyeCenter, Point glintCenter);
	void Eyerecognition(Mat inimg);
	string GetPassword();



	/*  멤버 변수 */
	int flagCount = 0;			//플래그 확인	
	int flagNum = 5;		//플래그 쓰레쉬홀드
	int beforePass = 0;		//직전 프레임 비밀번호

	vector <int> Password;
	
};

