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


extern int standardWidth;
extern int standardHeight;
extern int binaryThreshold;
extern int noiseFilter;
vector<Mat> CharactorSplit(Mat img);
void preProc(Mat & image);
void RemoveBlank(Mat & img);
Mat Dilation(Mat img);
