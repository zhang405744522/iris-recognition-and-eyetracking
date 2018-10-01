#include "main.h"

extern struct UserData User[50];

bool Data_load(void)
{
	Mat mData;
	ifstream ifs;
	string file_num = "1";
	ifs.open("user_data.txt");
	string name;
	int pattern, num, index = 0;
	while (ifs >> name >> pattern >> num)
	{
		User[index].name = name;
		User[index].pattern = pattern;
		User[index].num = num;

		for (int j = 0; j < User[index].num; j++)
		{
			file_num = to_string(j + 1);
			mData = imread("image\\" + User[index].name + file_num + ".jpg", 1);
			cout << "  бс " << User[index].name + file_num + ".jpg" << " Open" << endl;
			
			mData.copyTo(User[index].iris[j]);
			if (FindPupil(User[index].iris[j]))
			{
				if (FindIris(User[index].iris[j]))
				{
					vector<char> Code;
					GaborCode(User[index].iris[j],Code);
					User[index].irisCode.push_back(Code);
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		index++;
	}
	MAX_USER = index;

	ifs.close();
	system("PAUSE");
	return true;
}
bool Data_save(void)
{
	ofstream ofs("user_data.txt");
	if (ofs)
	{
		for (int i = 0; i < MAX_USER; i++)
		{
			ofs << User[i].name << " " << User[i].pattern << " " << User[i].num << endl;
		}
		ofs.close();
		return true;
	}
	else
		return false;
}