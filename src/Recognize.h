#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include "vector"
#include "algorithm"
#include "fstream"
#include "math.h"
#include "LittleBasic.h"
using namespace std;
using namespace cv;

enum{
	NoneWeight=0,
	EdgeWeighted=1,
	BackgroundWeighted=2
};
extern int standardWidth;
extern int standardHeight;
extern int binaryThreshold;
extern int noiseFilter;

string recognize(vector<Mat> subImage);
string Recognize(vector<Mat> subImage);
void thin(cv::Mat& src, cv::Mat& dst);
double ImageCompare(Mat img1,Mat img2,int distanceType=EdgeWeighted);
string Identify_3_5(Mat img,string firststep);
string Identify_0_D(Mat img,string firststep);
string Identify_8_B(Mat img,string firststep);
string Identify_2_Z(Mat img,string firststep);
