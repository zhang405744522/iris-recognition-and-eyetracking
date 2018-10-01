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
		cout << "\n ��������������������" << endl;
		//cout << "  ��\t\t\t\t  ��" << endl;
		cout << " �� 1 - Eye Security System\t     ��" << endl;
		cout << " �� 2 - Eye Security System Demo     ��" << endl;
		cout << " �� 3 - Add User\t\t     ��" << endl;
		cout << " �� 4 - Delete User\t\t     ��" << endl;
		cout << " �� 0 - Exit\t\t\t     ��" << endl;
		//cout << "  ��\t\t\t\t  �� " << endl;
		cout << " ��������������������" << endl;
		cout << "\n Command : ";
		cin >> menu;
		system("cls");
		if (menu == 1 || menu==2)
		{
			char data;
			int recog_count = 0,iris_flag=0;
			EyeTracking  cEyeTracking;
			if (menu == 1)
				capture.open(1);//ī�޶� ����
			else if (menu == 2)
				capture.open("2.mp4");//���� ����
			if (!capture.isOpened())
			{
				cout << "\n �� Error : Camera Disconnect " << endl;
				system("PAUSE");
				continue;
			}
			if (!Serial.OpenPort("COM3"))
			{
				cout << "\n �� Error : Arduino Disconnect " << endl;
				system("PAUSE");
				continue;
			}
			else
			{
				Serial.ConfigurePort(CBR_9600, 8, FALSE, NOPARITY, ONESTOPBIT); //��Ʈ �⺻���� �����Ѵ�.
				Serial.SetCommunicationTimeouts(0, 0, 0, 0, 0); //Timeout�� ����
				cout << "\n �� Arduino connect " << endl;
			}

			Serial.WriteByte('1');

			while (menu==1)
			{
				Serial.ReadByte(data);

				if (data == 's') // start
				{
					cout << "\n �� Capture Start " << endl;
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
										//cout << "  " << User[i].name << " �ν�" << endl;
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
								cout << " �� " << User[max_index].name << "�� ȫä �ν��� �����߽��ϴ�." << endl;
								iris_flag = 1;
								break;
							}
							else
								recog_count++;
							cout << " �� count = " << recog_count << endl;
							if (recog_count > 300)
							{
								cout << " �� ��ϵ��� ���� ����� �Դϴ�." << endl;
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
			// �Ƶ��̳� LED Ű�� �� ����
			Serial.WriteByte('2');
			cout << " �� �ü� ������ �����մϴ�" << endl;
			// �����ϸ�
			if (EyeTrackingFunction(cEyeTracking, 0,menu))
			{
				cout << " �� ���� ������ ��ġ �մϴ�" << endl;
				cout << " �� �ȳ��ϼ��� ���� ���Ƚ��ϴ�" << endl;
				Serial.WriteByte('3');
			}
			else {
				cout << " �� ���� ������ ��ġ���� �ʽ��ϴ�" << endl;
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
				cout << "\n �� Error : Camera Disconnect " << endl;
				system("PAUSE");
				continue;
			}

			cout << "\n �� ����� �̸��� �Է����ּ���\n  ";
			cin >> name;
			for (int i = 0; i < MAX_USER; i++)
			{
				if (name == User[i].name)
				{
					cout << " �� ���� �ι��� ���� �մϴ�"<< endl;
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
						cout << " �� ������ ã�ҽ��ϴ�" << endl;
						vector<char> Code;

						if (FindIris(mInputCam))
						{
							cout << " �� ȫä�� ã�ҽ��ϴ�\n\n  " << User[index].num << " ��° �̹����� ����Ǿ����ϴ�" << endl;
							User[index].num++;
							file_num = to_string(User[index].num);
							GaborCode(User[index].iris[User[index].num], Code);
							User[index].irisCode.push_back(Code);
							imwrite("image\\" + User[index].name + file_num + ".jpg", save_image);
							count++;
						}
					}
					else cout << " �� �ٽ� �õ����ּ���\n �� ȫä�� ��Ȯ�ϰ� �������մϴ�" << endl;
				}
				if (count == 5) break;
			}

			while (!flag)
			{
				cout << "\n\n�� ������ 2�ڸ� �Է����ּ���" << endl;
				cout << "\n\n����������������������" << endl;
				cout << "��\t\t\t\t\t��" << endl; //1
				cout << "��\t��\t\t\t��\t�� " << endl;//2
				cout << "��\t\t\t\t\t�� " << endl;//3
				cout << "��\t\t\t\t\t�� " << endl;//4
				cout << "��\t\t\t\t\t�� " << endl;//5
				cout << "��\t\t\t\t\t�� " << endl;//6
				cout << "��\t\t������\t\t�� " << endl;//7
				cout << "��\t\t��      ��\t\t�� " << endl;//8
				cout << "��\t\t��      ��\t\t�� " << endl;//9
				cout << "��\t\t������\t\t�� " << endl;//10
				cout << "��\t\t\t\t\t�� " << endl;//11
				cout << "��\t\t\t\t\t�� " << endl;//12
				cout << "��\t\t\t\t\t�� " << endl;//13
				cout << "��\t\t\t\t\t�� " << endl;//14
				cout << "��\t��\t\t\t��\t�� " << endl;//15
				cout << "��\t\t\t\t\t�� " << endl;//16
				cout << "����������������������" << endl;
				cout << "\n  �� Pattern : " << endl;

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
					cout << "\n\n  �� �ٽ� �Է����ּ���" << endl;
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

			cout << "\n �� ������ ����� �̸��� �Է����ּ���\n  ";
			cin >> name;
			for (int i = 0; i < MAX_USER; i++)
			{
				if (name == User[i].name)
				{
					cout << "\n �� ��ġ�ϴ� ����ڰ� �����մϴ�" << endl << endl;
					index = i;
					flag = 1;
					break;
				}
			}
			if (flag == 0)
			{
				cout << " �� ��ġ�ϴ� ����ڰ� �������� �ʽ��ϴ�" << endl << endl;
				system("PAUSE");
				continue;
			}
			
				cout << "\n �� ������ ������� ������ �Է����ּ���\n  pattern = ";
				cin >> pat;
				if (User[index].pattern == pat)
				{
					cout << "\n �� ������ ��ġ�մϴ�" << endl;
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
					cout << "\n �� ���������� �����Ǿ����ϴ�" << endl;
					system("PAUSE");
				}
				else
				{
					cout << "\n  �� ������ ��ġ���� �ʽ��ϴ�" << endl;
					system("PAUSE");
				}
		}
		else if (menu == 0)
		{
			Data_save();
			cout << "\n �� �����͸� ���������� �����Ͽ����ϴ�" << endl;
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
		cout << "\n �� Error : Camera Disconnect " << endl;
		exit(1);
	}

	//���� �о���� ���� �ݺ�
	for (;;)
	{
		capture.read(mInputCam);
		flip(mInputCam, mInputCam, 1);

		if (!mInputCam.empty() && cEyeTracking.GetPassword().size()<passwordSize) {
			cvtColor(mInputCam, mInputCam, CV_RGB2GRAY);	//�޾ƿ� ���� gray�� �ٲٱ�
			cEyeTracking.Eyerecognition(mInputCam);			//������ȯ�� ���� ������ 

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
			cout << endl << User[user].name << " " << User[user].num << " �� " << num+1 << "��° ȫä\n" << endl;
			for(int i=0; i < MAX_USER; i++)
				for (int j = 0; j < User[i].num; j++)
				{
					if (Recognition(User[user].irisCode[num], User[i].irisCode[j]))
					{
						User[i].recognition_count++;
						cout << "\t" << User[i].name << " �ν�"<< endl;
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
				cout << "\n\n\t\t" << User[max_index].name << "�� ȫä �ν��� �����߽��ϴ�." << endl;
			else
				cout << "\n\n\t\t��ϵ��� ���� ����� �Դϴ�" << endl;
			for (int i = 0; i < MAX_USER; i++)
			{
				User[i].recognition_count = 0;
			}
			system("PAUSE");
			}*/