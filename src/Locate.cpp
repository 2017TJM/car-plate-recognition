#include "stdafx.h"
#include "Locate.h"

double PixelColorDistance(Vec3b p1,Vec3b p2,double colorWeight[],int space)
{
	int B1=(int)p1[0];
	int G1=(int)p1[1];
	int R1=(int)p1[2];

	int B2=(int)p2[0];
	int G2=(int)p2[1];
	int R2=(int)p2[2];

	double distance;
	if (space==0)
	{
		double delta_B=(double)abs((B1-B2))/255;
		double delta_G=(double)abs((G1-G2))/255;
		double delta_R=(double)abs((R1-R2))/255;
		distance=colorWeight[0]*delta_B*delta_B+colorWeight[1]*delta_G*delta_G+colorWeight[2]*delta_R*delta_R;
		double CoeSum=colorWeight[0]+colorWeight[1]+colorWeight[2];
		distance=distance/CoeSum;
	}
	else if (space==1)
	{
		double delta_H=abs(B1-B2);
		delta_H=min(delta_H,180-delta_H)/90;
		double delta_S=(double)abs(G1-G2)/255;
		double delta_V=(double)abs(R1-R2)/255;
		distance=colorWeight[0]*delta_H*delta_H+colorWeight[1]*delta_S*delta_S+colorWeight[2]*delta_V*delta_V;
		double CoeSum=colorWeight[0]+colorWeight[1]+colorWeight[2];
		distance=distance/CoeSum;
	}
	distance=sqrt(distance);
	return distance;
}
double HightLightWhite(Vec3b p1,Vec3b p2)
{
	int B1=(int)p1[0];
	int G1=(int)p1[1];
	int R1=(int)p1[2];

	int B2=(int)p2[0];
	int G2=(int)p2[1];
	int R2=(int)p2[2];

	double distance;

	double delta_B=1-(double)abs((B1-B2))/255;
	double delta_G=(double)abs((G1-G2))/255;
	double delta_R=(double)abs((R1-R2))/255;
	distance=delta_B*delta_B+delta_G*delta_G+delta_R*delta_R;
	double CoeSum=3;
	distance=distance/CoeSum;
	return distance;
}
bool TestBorder(Mat img,int row,int col,int boxSize)
{
	if (row<0 || row>=img.rows || col<0 || col>=img.cols)
		return false;
	else
		return true;
	/*int offset=(boxSize-1)/2;
	int rows=img.rows;
	int cols=img.cols;
	int topLeft_row=row-offset;
	int topLeft_col=col-offset;
	for(int i=topLeft_row;i<topLeft_row+boxSize;i++)
	{
		for(int j=topLeft_col;j<topLeft_col+boxSize;j++)
		{
			if(i<0 || i>=rows)
				return false;
			if(j<0 || j>=cols)
				return false;
		}
	}
	return true;*/
}
Mat locatePlate(Mat img,int space,int ForDemo)
{
	ofstream write;
	write.open("show.txt");

	Mat operateImg;
	Mat gray;
	cvtColor(img,gray,COLOR_BGR2GRAY);
	Mat hsvImg;
	cvtColor(img,hsvImg, CV_BGR2HSV);
	vector<int> grayLevel;

	int boxSize=3; //5 is typical
	int offset=(boxSize-1)/2;

	operateImg=img;
	Vec3b standardBlue(240,0,0);
	double colorWeight[3]={1.5,1,1};

	if (space==1)
	{
		operateImg=hsvImg;
		Vec3b newStandardBlue(120,255,255); 
		standardBlue=newStandardBlue;
		colorWeight[0]=1.5;
		colorWeight[2]=1;
	}

	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++) 
		{
			double boxDistance=0;
			int topLeft_row=i-offset;
			int topLeft_col=j-offset;
			for(int r=topLeft_row;r<topLeft_row+boxSize;r++)
			{
				for(int c=topLeft_col;c<topLeft_col+boxSize;c++)
				{
					double distance;
					if(TestBorder(operateImg,r,c,boxSize)==false)
						distance=1;
					else
					{
						distance=PixelColorDistance(operateImg.at<Vec3b>(r,c),standardBlue,colorWeight,space);
					}
					boxDistance+=distance;
				}
			}
			//write<<boxDistance<<endl;
			double rate=boxDistance/(boxSize*boxSize);
			//rate=max(0.1,rate);
			double discrete_rate=floor(rate*10)/10;
			rate=discrete_rate=0 ? 0 : -log(discrete_rate);
			rate=min(rate,(double)1);
			int gray_level=(int)(rate*255);
			gray.at<uchar>(i,j)=gray_level;
			grayLevel.push_back(gray_level);
		}
	}
	imshow("HSV空间非线性映射为灰度图",gray);
	waitKey(0);
	vector<int> remainLevel;
	int remainLevelNum=5;
	for(int i=0;i<remainLevelNum;i++)
	{
		int maxLevel=vector_max_value(grayLevel);
		remainLevel.push_back(maxLevel);
		delete_value_x(grayLevel,maxLevel);
		Mat binary=gray.clone();
		for(int i=0;i<binary.rows;i++)
		{
			for(int j=0;j<binary.cols;j++) 
			{
				int remove=1;
				for(int k=0;k<remainLevel.size();k++)
					if((int)binary.at<uchar>(i,j)==remainLevel[k])
						remove=0;
				if(remove==1)
					binary.at<uchar>(i,j)=0;
				else
					binary.at<uchar>(i,j)=255;
			}
		}
		vector<RotatedRect> plateRect=findPlateRect(binary);
		if(plateRect.size()!=0)
		{
			Mat plate=CropRotatedBox(img,plateRect[0],"车牌",Horizontal,ForDemo);
			//imshow("车牌",plate);
			//StrengthenWhite(plate);	
			return plate;
		}
	}
	waitKey(0);
	return img;
	
}
void StrengthenWhite(Mat & img)
{
	//GaussianBlur(img,img,Size(3,3),0,0);
	Mat gray(img.size(),CV_8UC1);
	Vec3b standardBlue(255,0,0);
	Vec3b standardWhite(255,255,255); //
	/*vector<double> w;
	double colorWeight_b[3]={1,1,1};
	double colorWeight_w[3]={0,1,1};
	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++)
		{
			double BlueDistance=PixelColorDistance(img.at<Vec3b>(i,j),standardBlue,colorWeight_b,HSV_SPACE);
			double WhiteDistance=PixelColorDistance(img.at<Vec3b>(i,j),standardBlue,colorWeight_w,HSV_SPACE);
			w.push_back(WhiteDistance);
			double discrete_rate=floor(WhiteDistance*10)/10;
			double rate=discrete_rate=0 ? 0 : -log(discrete_rate);
			gray.at<uchar>(i,j)=(int)(rate*255);
		}
	}
	cout<<vector_max_value(w);*/
	


	int boxSize=1; //5 is typical
	int offset=(boxSize-1)/2;

	double colorWeight[3]={0,1,1};


	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++) 
		{
			double boxDistance=0;
			int topLeft_row=i-offset;
			int topLeft_col=j-offset;
			for(int r=topLeft_row;r<topLeft_row+boxSize;r++)
			{
				for(int c=topLeft_col;c<topLeft_col+boxSize;c++)
				{
					double distance;
					if(TestBorder(img,r,c,boxSize)==false)
						distance=1;
					else
					{
						double d1=PixelColorDistance(img.at<Vec3b>(r,c),standardWhite,colorWeight,0);
						//double d2=PixelColorDistance(img.at<Vec3b>(r,c),standardBlue,colorWeight,0);
						distance=d1;
					}
					boxDistance+=distance;
				}
			}
			//write<<boxDistance<<endl;
			double rate=boxDistance/(boxSize*boxSize);
			double discrete_rate=floor(rate*10)/10;
			rate=discrete_rate=0 ? 0 : -log(discrete_rate);
			rate=min(rate,(double)1);
			int gray_level=(int)(rate*255);
			gray.at<uchar>(i,j)=gray_level;
		}
	}
	imshow("white",gray);
	img=gray;
}
vector<RotatedRect> findPlateRect(Mat img)
{
	Mat drawing = Mat::zeros(img.size(), CV_8UC3 );
	vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
	findContours( img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	vector<RotatedRect> minRect;
	for( int i = 0; i < contours.size(); i++ )
    {
		RotatedRect box = minAreaRect( Mat(contours[i]) );
		double x_y_ratio=(double)box.size.width/(double)box.size.height;
		double y_x_ratio=1/x_y_ratio;
		int sizeThresh=1000;
		if(box.size.width*box.size.height>sizeThresh && (abs(x_y_ratio-4.7)<2.2 || abs(y_x_ratio-4.7)<2.2)) //先找出矩形
			minRect.push_back(box);
    }

	RotatedRect plateRect;
	if(minRect.size()==0)
	{
		//cout<<"第一次没有找到车牌！扩大范围继续寻找！"<<endl;
		return minRect;
	}
	else
		plateRect=minRect[0];
	for( int i = 0; i< contours.size(); i++ )
		drawContours( drawing, contours, i, Scalar(0, 0, 250), 1, 8, vector<Vec4i>(), 0, Point() );

	for( int i = 0; i< minRect.size(); i++ )
	{
		Point2f rect_points[4]; 
		plateRect.points( rect_points );
		for( int j = 0; j < 4; j++ )
			line( drawing, rect_points[j], rect_points[(j+1)%4], Scalar(0, 255, 0), 1, 8 );
	}
	namedWindow( "标记车牌区域", CV_WINDOW_AUTOSIZE );
    imshow( "标记车牌区域", drawing );
	//showRectInfo(plateRect);
	return minRect;
}
void showRectInfo(RotatedRect box)
{
	cout<<"center: ("<<box.center.x<<","<<box.center.y<<")"<<endl;
	cout<<"angle:"<<box.angle<<endl;
	cout<<"width:"<<box.size.width<<endl;
	cout<<"height:"<<box.size.height<<endl<<endl;
}
vector<vector<Point> > MarkContour(Mat img)
{
	Mat drawing = Mat::zeros(img.size(), CV_8UC3 );
	vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
	findContours( img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	for( int i = 0; i< contours.size(); i++ )
		drawContours( drawing, contours, i, Scalar(0, 0, 250), 1, 8, vector<Vec4i>(), 0, Point() );
	imshow( "轮廓", drawing );
	return contours;	
}
Mat CropRotatedBox(Mat img,RotatedRect box,string windowName,int cropType,int ForDemo) //先平移再旋转
{
	int RotateThreshold=2.2;
	Mat rot_mat( 2, 3, CV_32FC1 );
	Mat rotate(img);
	Point center = box.center;	
	double angle = box.size.width>box.size.height ? box.angle : 90 + box.angle;
	if(cropType==Vertical)
		angle = box.size.width<box.size.height ? box.angle : 90 + box.angle;
	if(abs(angle)>RotateThreshold)
	{
		double scale = 1;
		int newWidth=ceil((double)img.cols*1.4);
		int newHeight=ceil((double)img.rows*1.4);
		int delta_x=(newWidth-img.cols)/2;
		int delta_y=(newHeight-img.rows)/2;

		Mat offset = (Mat_<double>(2,3) << 1, 0, delta_x, 0, 1, delta_y); 
		warpAffine( img, rotate, offset, Size(newWidth,newHeight) );

		center.x+=delta_x;
		center.y+=delta_y;
		rot_mat = getRotationMatrix2D( center, angle, scale );	
		warpAffine( rotate, rotate, rot_mat, Size(newWidth,newHeight) );
	}
	if(ForDemo==1)
		imshow(windowName+"+旋转后",rotate);

	int realWidth=max(box.size.width,box.size.height);
	int realHeight=min(box.size.width,box.size.height);
	if(cropType==Vertical)
	{
		realWidth=min(box.size.width,box.size.height);
		realHeight=max(box.size.width,box.size.height);
	}

	int extend=1;
	Rect cropBox(center.x-realWidth/2,center.y-realHeight/2,realWidth+extend,realHeight+extend);
	Mat plate(rotate,cropBox);
	if(ForDemo==1)
		imshow(windowName+"+截出矩形",plate);
	return plate;	
}
void detectEdges(Mat src)
{
	int edgeThresh = 1;
	int lowThreshold=10;
	int const max_lowThreshold = 50;
	int ratio = 3;
	int kernel_size = 3;
	char* window_name = "Edge Map";
	Mat dst, detected_edges;
	dst.create( src.size(), src.type() );
	Mat src_gray=src.clone();
	namedWindow( window_name, CV_WINDOW_AUTOSIZE );
	
	blur( src_gray, detected_edges, Size(3,3) );

	Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

	dst = Scalar::all(0);

	src.copyTo( dst, detected_edges);
	imshow( window_name, dst );
}

vector<int> CountPixel_y(Mat image,int flag)
{
	int dot;
	if(flag==0)
		dot=255;
	else
		dot=0;
	vector<int> VerticalPixelCount;
	for(int i=0;i<image.cols;i++)
	{
		int count=0;
		for(int j=0;j<image.rows;j++)
			if((int)image.at<uchar>(j,i)==dot)
				count++;
		VerticalPixelCount.push_back(count);
	}
	return VerticalPixelCount;
}
vector<int> CountPixel_x(Mat image,int flag)
{
	int dot;
	if(flag==0)
		dot=255;
	else
		dot=0;
	vector<int> HorizontalPixelCount;
	for(int i=0;i<image.rows;i++)
	{
		int count=0;
		for(int j=0;j<image.cols;j++)
			if((int)image.at<uchar>(i,j)==dot)
				count++;
		HorizontalPixelCount.push_back(count);
	}
	return HorizontalPixelCount;
}
int CountBlockPixel(Mat image,int r1,int r2,int c1,int c2,int flag)
{
	int dot;
	if (flag==0)
		dot=255;
	else
		dot=0;
	int count=0;
	for(int i=r1;i<r2;i++)
	{
		for(int j=c1;j<c2;j++)
		{
			if((int)image.at<uchar>(i,j)==dot)
				count++;
		}
	}
	return count;
}

void Sharpen(Mat & img)
{
	Mat kern = (Mat_<char>(3,3) <<  0, -1,  0,
                               -1,  5, -1,
                                0, -1,  0);
	filter2D(img, img, img.depth(), kern );
	imshow("锐化",img);
}

void skeleton(Mat & img)
{
	Mat drawing = Mat::zeros(img.size(), CV_8UC3 );
	vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
	findContours( img, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	for( int i = 0; i< contours.size(); i++ )
		drawContours( drawing, contours, i, Scalar(0, 0, 250), 1, 8, vector<Vec4i>(), 0, Point() );
	
	
	Mat skeletonImg( img.size(), CV_32FC1 );
	
	for( int j = 0; j < img.rows; j++ )
    { 
		for( int i = 0; i < img.cols; i++ )
         { 
			int first=pointPolygonTest( contours[0], Point2f(i,j), true );
			int min=abs(first);
			for(int k=1;k<contours.size();k++)
			{
				int later=pointPolygonTest( contours[k], Point2f(i,j), true );
				if(abs(later)<min)
					min=abs(later);				
			}
			skeletonImg.at<float>(j,i) = -min; 
		}
    }

	double minVal; double maxVal;
    minMaxLoc( skeletonImg, &minVal, &maxVal, 0, 0, Mat() );
    minVal = abs(minVal); maxVal = abs(maxVal);
	for( int j = 0; j < img.rows; j++ )
     { for( int i = 0; i < img.cols; i++ )
          {
            if( skeletonImg.at<float>(j,i) < 0 )
              { drawing.at<Vec3b>(j,i)[0] = 255 - (int) abs(skeletonImg.at<float>(j,i))*255/minVal; }
            else if( skeletonImg.at<float>(j,i) > 0 )
              { drawing.at<Vec3b>(j,i)[2] = 255 - (int) skeletonImg.at<float>(j,i)*255/maxVal; }
            else
              { drawing.at<Vec3b>(j,i)[0] = 255; drawing.at<Vec3b>(j,i)[1] = 255; drawing.at<Vec3b>(j,i)[2] = 255; }
          }
     }
	imshow( "轮廓", drawing );
}

//---------------------------------------------------

//threshold(gray,binary,220,255,CV_THRESH_OTSU);
	//imshow("3",binary);

	/*vector<int> HorizontalPixelCount=CountPixel_x(binary);
	vector<int> VerticalPixelCount=CountPixel_y(binary);
	int pixNumThreshold_x=(int)(0.6*vector_max_value(HorizontalPixelCount));
	int pixNumThreshold_y=(int)(0.6*vector_max_value(VerticalPixelCount));
	vector<int> cropPos_x;
	vector<int> cropPos_y;
	for(int i=1;i<HorizontalPixelCount.size();i++)
	{
		if(HorizontalPixelCount[i]>=pixNumThreshold_x && HorizontalPixelCount[i-1]<pixNumThreshold_x)
			cropPos_y.push_back(i);
		if(HorizontalPixelCount[i]<pixNumThreshold_x && HorizontalPixelCount[i-1]>=pixNumThreshold_x)
			cropPos_y.push_back(i);
	}
	for(int i=1;i<VerticalPixelCount.size();i++)
	{
		if(VerticalPixelCount[i]>=pixNumThreshold_y && VerticalPixelCount[i-1]<pixNumThreshold_y)
			cropPos_x.push_back(i);
		if(VerticalPixelCount[i]<pixNumThreshold_y && VerticalPixelCount[i-1]>=pixNumThreshold_y)
			cropPos_x.push_back(i);
	}
	
	vector<Rect> availableBlock;
	int BlockPixelsThreshold=20;
	if(cropPos_x.size()>1 && cropPos_y.size()>1)
	{
		for(int i=1;i<cropPos_x.size();i++)
			for(int j=1;j<cropPos_y.size();j++)
			{
				int r1=cropPos_y[j-1];
				int r2=cropPos_y[j];
				int c1=cropPos_x[i-1];
				int c2=cropPos_x[i];
				if(CountBlockPixel(binary,r1,r2,c1,c2)>BlockPixelsThreshold)
				{
					Rect rect(c1,r1,c2-c1,r2-r1);
					availableBlock.push_back(rect);
				}
			}
	}
	else
		cout<<"I'm sorry , the algorithm failed.";
	vector<Mat> locateResult;
	for(int i=0;i<availableBlock.size();i++)
	{
		Mat crop(img,availableBlock[i]);
		locateResult.push_back(crop);
	}
	img= locateResult[0];*/
	//imshow("fuck",crop);



//仿射变换
/*char* source_window = "Source image";
	char* warp_window = "Warp";
	char* warp_rotate_window = "Warp + Rotate";

   Mat rot_mat( 2, 3, CV_32FC1 );
   Mat warp_mat( 2, 3, CV_32FC1 );
   Mat src, warp_dst, warp_rotate_dst;

   /// 加载源图像
   src = img;

  

   /// 计算绕图像中点顺时针旋转50度缩放因子为0.6的旋转矩阵
   Point center = Point( img.cols/2, img.rows/2 );
   double angle = -50.0;
   double scale = 1;

   /// 通过上面的旋转细节信息求得旋转矩阵
   rot_mat = getRotationMatrix2D( center, angle, scale );

   /// 旋转已扭曲图像
   int newWidth=ceil((double)img.cols*2);
   int newHeight=ceil((double)img.rows*2);
   warpAffine( src, warp_rotate_dst, rot_mat, Size(newWidth,newHeight) );

   /// 显示结果
   namedWindow( source_window, CV_WINDOW_AUTOSIZE );
   imshow( source_window, src );


   namedWindow( warp_rotate_window, CV_WINDOW_AUTOSIZE );
   imshow( warp_rotate_window, warp_rotate_dst );*/
