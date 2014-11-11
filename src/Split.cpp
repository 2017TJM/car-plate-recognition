#include "stdafx.h"
#include "Split.h"
#include "Locate.h"

Mat Dilation(Mat img)
{
	Mat erosion_dst;
	int erosion_elem = 0;
	int erosion_size = 1;
	int const max_elem = 2;
	int const max_kernel_size = 21;
    int erosion_type;
  if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
  else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
  else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

    Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );
  
  dilate( img, erosion_dst, element );
  imshow( "erosion Demo", erosion_dst );
  return erosion_dst;
}

void sortRect(vector<RotatedRect> & minRect,int order=1) //按中心点的顺序从左往右排列
{
	int num=minRect.size();
	RotatedRect temp;
	for(int i=num;i>0;i--)
	{
		for(int j=0;j<i-1;j++)
		{
			if((minRect[j].center.x-minRect[j+1].center.x)*order>0)
			{
				temp=minRect[j];
				minRect[j]=minRect[j+1];
				minRect[j+1]=temp;
			}
		}
	}
}
vector<Mat> CharactorSplit(Mat img)
{
	vector<Mat> subImage;
	
	Mat original=img.clone(); //findContours执行后原图像已损坏
	double height_width_ratio=1.2;
	Mat drawing = Mat::zeros(img.size(), CV_8UC3 );
	vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
	findContours( img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	for( int i = 0; i< contours.size(); i++ )
		drawContours( drawing, contours, i, Scalar(0, 0, 250), 1, 8, vector<Vec4i>(), 0, Point() );

	vector<RotatedRect> minRect;
	
	int maxHeight=0;
	for( int i = 0; i < contours.size(); i++ )
    {
		RotatedRect box = minAreaRect( Mat(contours[i]) );
		int w=abs(box.angle)<45 ? box.size.width : box.size.height;
		int h=abs(box.angle)<45 ? box.size.height : box.size.width;
		maxHeight=h>maxHeight ? h : maxHeight;
    }
	for( int i = 0; i < contours.size(); i++ )
    {
		RotatedRect box = minAreaRect( Mat(contours[i]) );
		int w=abs(box.angle)<45 ? box.size.width : box.size.height;
		int h=abs(box.angle)<45 ? box.size.height : box.size.width;
		double y_x_ratio=(double)h/w;
		if(h>maxHeight*0.75 && y_x_ratio>height_width_ratio)
			minRect.push_back(box);
    }

	for(int i=minRect.size();i>0;i--) //去掉中心重叠的
	{
		for(int j=0;j<i-1;j++)
		{
			int x1=minRect[j].center.x;
			int x2=minRect[j+1].center.x;
			int width1=min(minRect[j].size.width,minRect[j].size.height);
			int width2=min(minRect[j+1].size.width,minRect[j+1].size.height);
			int minInterval=max(width1,width2)/2;
			if(abs(x1-x2)<minInterval)
			{
				int removeId=width1>width2 ? j+1:j;
				delete_pos_i(minRect,removeId);
				i--;
			}
		}
	}
	sortRect(minRect,-1);
	
	vector<int> widths;
	for(int i=0;i<minRect.size();i++)
	{
		int width=min(minRect[i].size.width,minRect[i].size.height);
		widths.push_back(width);
	}
	int medianWidth=widths[widths.size()/2];

	vector<RotatedRect>::iterator it = minRect.begin();
	if(minRect[0].center.x-minRect[1].center.x<medianWidth)
		minRect.erase(it);

	int rearNum=min(5,(int)minRect.size()-1);
	for( int i = rearNum; i>=0; i-- ) //取后面6个字符
	{
		//showRectInfo(minRect[i]);
		subImage.push_back(CropRotatedBox(original,minRect[i],str(i),Vertical));
		Point2f rect_points[4]; 
		minRect[i].points( rect_points );
		for( int j = 0; j < 4; j++ )
			line( drawing, rect_points[j], rect_points[(j+1)%4], Scalar(0, 255, 0), 1, 8 );
	}

	imshow( "定位字符", drawing );
	int cropDistance=100;
	for(int i=0;i<subImage.size();i++) 
	{
// 		RemoveBlank(subImage[i]);
		resize(subImage[i],subImage[i],Size(standardWidth,standardHeight),0,0);
		threshold(subImage[i],subImage[i],200,255,CV_THRESH_OTSU);
		
		string CurrentWindowName=str(i);
		namedWindow(CurrentWindowName);
		moveWindow(CurrentWindowName,i*cropDistance,0);
		imshow(CurrentWindowName,subImage[i]);
	}
	return subImage;
}


void preProc(Mat & image) //去掉亮条和小斑点以及铆钉，程序基于黑底白字识别，所以全部处理为黑色
{
	vector<int> HorizontalPixelCount=CountPixel_x(image);
	vector<int> VerticalPixelCount=CountPixel_y(image);
	vector<int> brightPos;
	for(int i=0;i<HorizontalPixelCount.size();i++)
	{
		if(HorizontalPixelCount[i]>(int)image.cols*0.8)
			brightPos.push_back(i);
		if(HorizontalPixelCount[i]<image.cols/11+noiseFilter)
			brightPos.push_back(i);
	}
	for(int i=0;i<brightPos.size();i++)
		for(int j=0;j<image.cols;j++)
			image.at<uchar>(brightPos[i],j)=0;
	/*brightPos.clear();
	for(int i=0;i<VerticalPixelCount.size();i++)
	{
		if(VerticalPixelCount[i]>image.rows-noiseFilter)
			brightPos.push_back(i);
		if(VerticalPixelCount[i]<noiseFilter)
			brightPos.push_back(i);
	}
	for(int i=0;i<brightPos.size();i++)
		for(int j=0;j<image.rows;j++)
			image.at<uchar>(j,brightPos[i])=0;*/
}
void RemoveBlank(Mat & img)
{
	Rect box;
	box.x=0;
	box.width=img.cols;
	int pixNumThresh_x=noiseFilter;
	int pixNumThresh_y=noiseFilter;
	vector<int> HorizontalPixelCount=CountPixel_x(img);
	vector<int> VerticalPixelCount=CountPixel_y(img);
	for(int i=0;i<HorizontalPixelCount.size();i++)
	{
		if(HorizontalPixelCount[i]>0)
		{
			box.y=i;
			break;
		}
	}
	for(int i=HorizontalPixelCount.size()-1;i>=0;i--)
	{
		if(HorizontalPixelCount[i]>0)
		{
			box.height=i-box.y;
			break;
		}
	}

	for(int i=0;i<VerticalPixelCount.size();i++)
	{
		if(VerticalPixelCount[i]>0)
		{
			box.x=i;
			break;
		}
	}
	for(int i=VerticalPixelCount.size()-1;i>=0;i--)
	{
		if(VerticalPixelCount[i]>0)
		{
			box.width=i-box.x;
			break;
		}
	}
	Mat crop(img,box);
	img=crop;
}

//--------------------------------------------------------------

//vector<int> VerticalPixelCount=CountPixel_y(img); //crop vertically
	//vector<int> separatePos;
	//separatePos.push_back(0);
	//int pixNumThresh_y=img.rows/10;
	//for(int i=1;i<VerticalPixelCount.size();i++)
	//{
	//	if(VerticalPixelCount[i-1]<pixNumThresh_y && VerticalPixelCount[i]>=pixNumThresh_y)
	//		separatePos.push_back(i);
	//	if(VerticalPixelCount[i-1]>=pixNumThresh_y && VerticalPixelCount[i]<pixNumThresh_y)
	//		separatePos.push_back(i);
	//}	
	//separatePos.push_back(img.cols);
	//Rect box;
	//
	//int failSplitThreshold=50;
	//int cropDistance=100;
	//
	//for(int pos=1;pos<separatePos.size();pos++)
	//{
	//	box.x=separatePos[pos-1];
	//	box.y=0;
	//	box.width=separatePos[pos]-separatePos[pos-1];
	//	box.height=img.rows;		
	//	int count=0;
	//	for(int i=box.x;i<box.x+box.width;i++)
	//	{		
	//		for(int j=0;j<img.rows;j++)
	//			if((int)img.at<uchar>(j,i)==255)
	//				count++;
	//	}
	//	if(count>failSplitThreshold)
	//	{
	//		Mat crop(img,box);			
	//		subImage.push_back(crop);			
	//	}
	//}
	//for(int i=0;i<subImage.size();i++) //crop horizontally
	//{
	//	Rect box;
	//	box.x=0;
	//	box.width=subImage[i].cols;
	//	int pixNumThresh_x=subImage[i].cols/10+noiseFilter;
	//	vector<int> HorizontalPixelCount=CountPixel_x(subImage[i]);
	//	for(int i=0;i<HorizontalPixelCount.size();i++)
	//	{
	//		if(HorizontalPixelCount[i]>=pixNumThresh_x)
	//		{
	//			box.y=i;
	//			break;
	//		}
	//	}
	//	for(int i=HorizontalPixelCount.size()-1;i>=0;i--)
	//	{
	//		if(HorizontalPixelCount[i]>=pixNumThresh_x)
	//		{
	//			box.height=i-box.y;
	//			break;
	//		}
	//	}
	//	Mat crop(subImage[i],box);
	//	subImage[i]=crop;
	//	resize(subImage[i],subImage[i],Size(standardWidth,standardHeight));
	//	threshold(subImage[i],subImage[i],binaryThreshold,255,CV_THRESH_BINARY);
	//	string CurrentWindowName=str(i);
	//	namedWindow(CurrentWindowName);
	//	moveWindow(CurrentWindowName,i*cropDistance,0);
	//	imshow(CurrentWindowName,subImage[i]);
	//}
