#pragma once
#include"main.h"

class EyeTracking {
public:
	void Direction(Point eyeCenter, Point glintCenter);
	void Eyerecognition(Mat inimg);
	string GetPassword();



	/*  ��� ���� */
	int flagCount = 0;			//�÷��� Ȯ��	
	int flagNum = 5;		//�÷��� ������Ȧ��
	int beforePass = 0;		//���� ������ ��й�ȣ

	vector <int> Password;
	
};

