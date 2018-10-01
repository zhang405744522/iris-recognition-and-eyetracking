#include "main.h"
#include"EyeTracking.h"
struct UserData User[50];

CSerialPort Serial;
bool EyeTrackingFunction(EyeTracking cEyeTracking,int UserStructIndex, int mode);

VideoCapture capture;
Mat mInputCam;

int MAX_USER=0;

void main()
{
	system("color f0");
	system("mode con:lines=18 cols=40");
	system("title Iris Recognition , Eye Tracking");
	int menu = 0,iris_flag=0;
	Data_load();
	while (1)
	{
		destroyAllWindows();
		system("cls");
		cout << "\n Total User : " << MAX_USER << endl;
		cout << "\n ■■■■■■■■■■■■■■■■■■■" << endl;
		//cout << "  ■\t\t\t\t  ■" << endl;
		cout << " ■ 1 - Eye Security System\t     ■" << endl;
		cout << " ■ 2 - Eye Security System Demo     ■" << endl;
		cout << " ■ 3 - Add User\t\t     ■" << endl;
		cout << " ■ 4 - Delete User\t\t     ■" << endl;
		cout << " ■ 0 - Exit\t\t\t     ■" << endl;
		//cout << "  ■\t\t\t\t  ■ " << endl;
		cout << " ■■■■■■■■■■■■■■■■■■■" << endl;
		cout << "\n Command : ";
		cin >> menu;
		system("cls");
		if (menu == 1 || menu==2)
		{
			char data;
			int recog_count = 0,iris_flag=0;
			EyeTracking  cEyeTracking;
			if (menu == 1)
				capture.open(1);//카메라 오픈
			else if (menu == 2)
				capture.open("2.mp4");//영상 오픈
			if (!capture.isOpened())
			{
				cout << "\n ■ Error : Camera Disconnect " << endl;
				system("PAUSE");
				continue;
			}
			if (!Serial.OpenPort("COM3"))
			{
				cout << "\n ■ Error : Arduino Disconnect " << endl;
				system("PAUSE");
				continue;
			}
			else
			{
				Serial.ConfigurePort(CBR_9600, 8, FALSE, NOPARITY, ONESTOPBIT); //포트 기본값을 설정한다.
				Serial.SetCommunicationTimeouts(0, 0, 0, 0, 0); //Timeout값 설정
				cout << "\n ■ Arduino connect " << endl;
			}

			Serial.WriteByte('1');

			while (menu==1)
			{
				Serial.ReadByte(data);

				if (data == 's') // start
				{
					cout << "\n ■ Capture Start " << endl;
					break;
				}
			}
			capture.read(mInputCam);
			int max_index = 0, max = 0;

			while (1)
			{
				capture.read(mInputCam);
				flip(mInputCam, mInputCam, 1);
				resize(mInputCam, mInputCam, Size(640, 480));
				imshow("m", mInputCam);
				moveWindow("m", 0, 10);
				if (waitKey(33))
				{
					if (FindPupil(mInputCam))
					{
						if (FindIris(mInputCam))
						{
							Mat temp_iris;
							resize(mInputCam, temp_iris, Size(600, 80));
							imshow("iris", temp_iris);
							moveWindow("iris", 500, 150);
							vector<char> Code;
							GaborCode(mInputCam, Code);

							for (int i = 0; i < MAX_USER; i++)
								for (int j = 0; j < User[i].num; j++)
								{
									if (Recognition(Code, User[i].irisCode[j]))
									{
										User[i].recognition_count++;
										//cout << "  " << User[i].name << " 인식" << endl;
									}
								}
							max = 0;
							max_index = 0;

							for (int i = 0; i < MAX_USER; i++)
							{
								if (max < User[i].recognition_count)
								{
									max = User[i].recognition_count;
									max_index = i;
								}
							}
							for (int i = 0; i < MAX_USER; i++)
							{
								User[i].recognition_count = 0;
							}
							if (max > 0)
							{
								cout << " ■ " << User[max_index].name << "님 홍채 인식이 성공했습니다." << endl;
								iris_flag = 1;
								break;
							}
							else
								recog_count++;
							cout << " ■ count = " << recog_count << endl;
							if (recog_count > 300)
							{
								cout << " ■ 등록되지 않은 사용자 입니다." << endl;
								iris_flag = 0;
								Serial.WriteByte('4');
								Serial.ClosePort();
								break;
							}

						}
					}
				}

			}
			if (iris_flag == 0) continue;
			// 아두이노 LED 키는 거 전송
			Serial.WriteByte('2');
			cout << " ■ 시선 추적을 시작합니다" << endl;
			// 성공하면
			if (EyeTrackingFunction(cEyeTracking, 0,menu))
			{
				cout << " ■ 동공 패턴이 일치 합니다" << endl;
				cout << " ■ 안녕하세요 문이 열렸습니다" << endl;
				Serial.WriteByte('3');
			}
			else {
				cout << " ■ 동공 패턴이 일치하지 않습니다" << endl;
				Serial.WriteByte('4');
			}
			Serial.ClosePort();
			system("PAUSE");
		}
		else if (menu == 3)
		{
			Mat save_image;
			string name, file_num = "1";
			int index= MAX_USER,pat=0,flag=0,count=0;
			capture.open(1);
			if (!capture.isOpened())
			{
				cout << "\n ■ Error : Camera Disconnect " << endl;
				system("PAUSE");
				continue;
			}

			cout << "\n ■ 사용자 이름을 입력해주세요\n  ";
			cin >> name;
			for (int i = 0; i < MAX_USER; i++)
			{
				if (name == User[i].name)
				{
					cout << " ■ 동일 인물이 존재 합니다"<< endl;
					system("PAUSE");
					index = i;
					flag = 1;
					break;
				}
			}
			system("cls");
			User[index].name = name;
			capture.read(mInputCam);

			while (1)
			{
				capture.read(mInputCam);
				flip(mInputCam, mInputCam, 1);
				imshow("m", mInputCam);
				moveWindow("m", 0, 10);
				mInputCam.copyTo(save_image);
				if (waitKey(33) >= 0)
				{
					if (FindPupil(mInputCam))
					{
						cout << " ■ 동공을 찾았습니다" << endl;
						vector<char> Code;

						if (FindIris(mInputCam))
						{
							cout << " ■ 홍채를 찾았습니다\n\n  " << User[index].num << " 번째 이미지가 저장되었습니다" << endl;
							User[index].num++;
							file_num = to_string(User[index].num);
							GaborCode(User[index].iris[User[index].num], Code);
							User[index].irisCode.push_back(Code);
							imwrite("image\\" + User[index].name + file_num + ".jpg", save_image);
							count++;
						}
					}
					else cout << " ■ 다시 시도해주세요\n ■ 홍채는 정확하게 찍혀야합니다" << endl;
				}
				if (count == 5) break;
			}

			while (!flag)
			{
				cout << "\n\n■ 패턴을 2자리 입력해주세요" << endl;
				cout << "\n\n■■■■■■■■■■■■■■■■■■■■■" << endl;
				cout << "■\t\t\t\t\t■" << endl; //1
				cout << "■\t①\t\t\t②\t■ " << endl;//2
				cout << "■\t\t\t\t\t■ " << endl;//3
				cout << "■\t\t\t\t\t■ " << endl;//4
				cout << "■\t\t\t\t\t■ " << endl;//5
				cout << "■\t\t\t\t\t■ " << endl;//6
				cout << "■\t\t■■■■■\t\t■ " << endl;//7
				cout << "■\t\t■      ■\t\t■ " << endl;//8
				cout << "■\t\t■      ■\t\t■ " << endl;//9
				cout << "■\t\t■■■■■\t\t■ " << endl;//10
				cout << "■\t\t\t\t\t■ " << endl;//11
				cout << "■\t\t\t\t\t■ " << endl;//12
				cout << "■\t\t\t\t\t■ " << endl;//13
				cout << "■\t\t\t\t\t■ " << endl;//14
				cout << "■\t③\t\t\t④\t■ " << endl;//15
				cout << "■\t\t\t\t\t■ " << endl;//16
				cout << "■■■■■■■■■■■■■■■■■■■■■" << endl;
				cout << "\n  ■ Pattern : " << endl;

				cin >> pat;

				if ((pat % 10 >= 1) && (pat % 10 <= 4) && (pat / 10 >= 1) && (pat / 10 <= 4) && (pat % 10 != pat / 10))
				{
					User[index].pattern = pat;
					MAX_USER++;
					break;
				}
				else
				{
					system("cls");
					cout << "\n\n  ■ 다시 입력해주세요" << endl;
				}
			}
			Data_save();
			//Data_load();
			system("PAUSE");
		}
		else if (menu == 4)
		{
			string name, file_num = "1";
			int index = MAX_USER, pat = 0, flag = 0, count = 0;

			cout << "\n ■ 삭제할 사용자 이름을 입력해주세요\n  ";
			cin >> name;
			for (int i = 0; i < MAX_USER; i++)
			{
				if (name == User[i].name)
				{
					cout << "\n ■ 일치하는 사용자가 존재합니다" << endl << endl;
					index = i;
					flag = 1;
					break;
				}
			}
			if (flag == 0)
			{
				cout << " ■ 일치하는 사용자가 존재하지 않습니다" << endl << endl;
				system("PAUSE");
				continue;
			}
			
				cout << "\n ■ 삭제할 사용자의 패턴을 입력해주세요\n  pattern = ";
				cin >> pat;
				if (User[index].pattern == pat)
				{
					cout << "\n ■ 패턴이 일치합니다" << endl;
					for (int i = index; i < MAX_USER; i++)
					{
						*User[i].iris = *User[i + 1].iris;
						User[i].num = User[i + 1].num;
						User[i].name = User[i + 1].name;
						User[i].irisCode = User[i + 1].irisCode;
						User[i].pattern = User[i + 1].pattern;
					}
					MAX_USER--;
					Data_save();
					cout << "\n ■ 정상적으로 삭제되었습니다" << endl;
					system("PAUSE");
				}
				else
				{
					cout << "\n  ■ 패턴이 일치하지 않습니다" << endl;
					system("PAUSE");
				}
		}
		else if (menu == 0)
		{
			Data_save();
			cout << "\n ■ 데이터를 성공적으로 저장하였습니다" << endl;
			system("PAUSE");
			exit(1);
		}
		
	}
}


bool EyeTrackingFunction(EyeTracking cEyeTracking, int UserStructIndex ,int mode)
{

	double frame_rate = capture.get(CV_CAP_PROP_FPS);
	int nDelay = 1000 / frame_rate;
	string savedPattern;
	string inputPattern;
	int passwordSize;
	if (mode == 1)
		passwordSize = 2;
	else if(mode == 2)
		passwordSize = 10;

	if (!capture.isOpened())
	{
		cout << "\n ■ Error : Camera Disconnect " << endl;
		exit(1);
	}

	//영상 읽어오는 동안 반복
	for (;;)
	{
		capture.read(mInputCam);
		flip(mInputCam, mInputCam, 1);

		if (!mInputCam.empty() && cEyeTracking.GetPassword().size()<passwordSize) {
			cvtColor(mInputCam, mInputCam, CV_RGB2GRAY);	//받아온 영상 gray로 바꾸기
			cEyeTracking.Eyerecognition(mInputCam);			//허프변환을 통해 원검출 

			if (waitKey(33) >= 0)
				break;
		}
		else if (!mInputCam.empty() && cEyeTracking.GetPassword().size() == passwordSize) {
			savedPattern = to_string(User[UserStructIndex].pattern);
			inputPattern = cEyeTracking.GetPassword();
			if (inputPattern == savedPattern)
				return true;
			else
				return false;


		}
		else
			break;
	}

}

/*else if (menu == 4)
		{
			int user, num;
			cout << "user = ";
			cin >> user;
			cout << "num = ";
			cin >> num;
			if (num > User[user].num)
				continue;
			cout << endl << User[user].name << " " << User[user].num << " 중 " << num+1 << "번째 홍채\n" << endl;
			for(int i=0; i < MAX_USER; i++)
				for (int j = 0; j < User[i].num; j++)
				{
					if (Recognition(User[user].irisCode[num], User[i].irisCode[j]))
					{
						User[i].recognition_count++;
						cout << "\t" << User[i].name << " 인식"<< endl;
					}
				}
			int max=0, max_index=0;
			for (int i = 0; i < MAX_USER; i++)
			{
				if (max < User[i].recognition_count)
				{
					max = User[i].recognition_count;
					max_index = i;
				}
			}
			if (max > 0)
				cout << "\n\n\t\t" << User[max_index].name << "님 홍채 인식이 성공했습니다." << endl;
			else
				cout << "\n\n\t\t등록되지 않은 사용자 입니다" << endl;
			for (int i = 0; i < MAX_USER; i++)
			{
				User[i].recognition_count = 0;
			}
			system("PAUSE");
			}*/