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

enum
{
	BGR_SPACE=0,
	HSV_SPACE=1
};
enum
{
	Vertical=0,
	Horizontal=1
};
extern int standardWidth;
extern int standardHeight;
extern int binaryThreshold;
extern int noiseFilter;

Mat locatePlate(Mat img,int space=HSV_SPACE,int ForDemo=0);
void detectEdges(Mat src);
vector<RotatedRect> findPlateRect(Mat img);
Mat CropRotatedBox(Mat img,RotatedRect box,string windowName,int cropType=Horizontal,int ForDemo=0);
vector<vector<Point> > MarkContour(Mat img);
void showRectInfo(RotatedRect box);
double PixelDistance(Vec3b p1,Vec3b p2,double colorWeight[],int space=HSV_SPACE);
vector<int> CountPixel_x(Mat image,int flag=0);
vector<int> CountPixel_y(Mat image,int flag=0); //default:white as a dot
int CountBlockPixel(Mat image,int r1,int r2,int c1,int c2,int flag=0); //区间前闭后开
bool TestBorder(Mat img,int row,int col,int boxSize=1);
void Sharpen(Mat & img);
void skeleton(Mat & img);
void StrengthenWhite(Mat & img);
