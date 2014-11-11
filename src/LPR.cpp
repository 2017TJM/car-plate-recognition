#include "stdafx.h"
#include "windows.h" 
#include "LittleBasic.h"
#include "Locate.h"
#include "Split.h"
#include "recognize.h"

void demo();
void evaluate();
void ResumeConsoleColor();
void ChangeConsoleColor();

//set parameters
int standardWidth=15;
int standardHeight=30;
int binaryThreshold=36;
int noiseFilter=3;

int _tmain(int argc, _TCHAR* argv[])
{
	demo();
	evaluate();

// 	Mat raw;
// 	Mat gray;
// 	Mat binary;
// 	vector<Mat> subImage;
// 	string plateNumber;
// 
// 	raw=imread("TestSet/9.jpg");
// 	imshow("原图",raw);
// 	raw=locatePlate(raw);
// 	cvtColor(raw,gray,COLOR_BGR2GRAY);
// 	imshow("gray",raw);
// 	threshold(gray,binary,binaryThreshold,255,CV_THRESH_OTSU);
// 	preProc(binary);
// 	imshow("二值化",binary);
// 	subImage=CharactorSplit(binary);
// 	plateNumber=Recognize(subImage);
// 	plateNumber.insert(1,"·");
// 	cout<<plateNumber<<endl;
// 	waitKey(0);
// 	return 0;
}

void demo()
{
	Mat raw;
	Mat gray;
	Mat binary;
	vector<Mat> subImage;
	string plateNumber;
	ResumeConsoleColor();
	cout<<"-----Here is a simple introduction to zhoutong's LPR system-----"<<endl<<endl;
	ChangeConsoleColor();
	cout<<"1.The first step:read image from the file"<<endl;
	ResumeConsoleColor();
	cout<<"the figure \"original figure\" is a news photograph."<<endl;
	cout<<"I pick it because it is appropriate for the demostration of tilt rectification"<<endl<<endl;
	raw=imread("TestSet/5.jpg");
	imshow("原图",raw);
	waitKey(0);

	ChangeConsoleColor();
	cout<<"2.The second step:locate the plate area in HSV space"<<endl;
	ResumeConsoleColor();
	cout<<"After a \"log fuction\" projection,the closer a pixle's color is to standarad blue,the brighter it gets when it comes to gray image"<<endl;
	cout<<"In other words,the adjacence is amplified while the farness is inhibited"<<endl<<endl;
	ChangeConsoleColor();
	cout<<"3.The third step:rotate the image and crop the plate area"<<endl;
	ResumeConsoleColor();
	cout<<"In most cases,plate area is only slightly tilted,due to the fact that affine transformation is always accompanied by the loss of infomation amount,"<<endl;
	cout<<"always performing a tilt rectification is not suggested"<<endl<<endl;
	raw=locatePlate(raw,1,1);
	waitKey(0);

	ChangeConsoleColor();
	cout<<"4.The forth step:separate each charator"<<endl;
	ResumeConsoleColor();
	cout<<"opencv has provided a awesome method for doing such stuff——minRectArea,which is able to split very accurately"<<endl<<endl;
	cvtColor(raw,gray,COLOR_BGR2GRAY);
	threshold(gray,binary,binaryThreshold,255,CV_THRESH_OTSU);
	preProc(binary);
	imshow("二值化",binary);
	subImage=CharactorSplit(binary);
	waitKey(0);

	ChangeConsoleColor();
	cout<<"5.The fifth step:resize each character and look for its match in the models "<<endl;
	ResumeConsoleColor();
	cout<<"the model is a key factor is this step,it is safe to say a better model guarantee you a better recognition result"<<endl<<endl;
	plateNumber=Recognize(subImage);
	waitKey(0);

	ChangeConsoleColor();
	cout<<"6.The sixth step:outcome"<<endl;
	ResumeConsoleColor();
	cout<<"Accroding to ML principal,we get a string."<<endl;
	cout<<"but before print the final result,a little adjustment is needed to improve possibility"<<endl;
	plateNumber.insert(1,"·");
	cout<<"In the end,the plate number is:"<<endl<<endl;
	cout<<plateNumber<<endl;
	waitKey(0);

	cout<<"thank you!if you have any idea or criticism,do not hesitate to tell me."<<endl<<endl;
}

void ChangeConsoleColor()
{
	HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_GREEN);//设置为红色
}
void ResumeConsoleColor()
{
	HANDLE handle=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);//设置为白色
}

void evaluate()
{
	Mat raw;
	Mat gray;
	Mat binary;
	vector<Mat> subImage;
	string plateNumber;

	for(int i=1;i<=10;i++)
	{
		string imageId=str(i);
		raw=imread("TestSet/"+imageId+".jpg");
		imshow("原图",raw);
		raw=locatePlate(raw);
		cvtColor(raw,gray,COLOR_BGR2GRAY);
		imshow("gray",raw);
		threshold(gray,binary,binaryThreshold,255,CV_THRESH_OTSU);
		preProc(binary);
		//imshow("二值化",binary);
		subImage=CharactorSplit(binary);
		plateNumber=Recognize(subImage);
		plateNumber.insert(1,"·");
		cout<<imageId<<" : "<<plateNumber<<endl;
	}
}
