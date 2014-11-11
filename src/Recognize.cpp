#include "stdafx.h"
#include "recognize.h"
#include "Model.h"

bool TestBorder(Mat img,int row,int col)
{
	if (row<0 || row>=img.rows || col<0 || col>=img.cols)
		return false;
	else
		return true;
}
void thin(cv::Mat& src, cv::Mat& dst)
{
    if(src.type()!=CV_8UC1)
    {
        printf("只能处理二值或灰度图像\n");
        return;
    }
    //非原地操作时候，copy src到dst
    if(dst.data!=src.data)
    {
        src.copyTo(dst);
    }

    int i, j, n;
    int width, height;
    //之所以减1，是方便处理8邻域，防止越界
    width = src.cols -1;
    height = src.rows -1;
    int step = src.step;
    int  p2,p3,p4,p5,p6,p7,p8,p9;
    uchar* img;
    bool ifEnd;
    cv::Mat tmpimg;
    int dir[4] = {-step, step, 1, -1};

    while(1)
    {
        //分四个子迭代过程，分别对应北，南，东，西四个边界点的情况
        ifEnd = false;
        for(n =0; n < 4; n++)
        {
            dst.copyTo(tmpimg);
            img = tmpimg.data;
            for(i = 1; i < height; i++)
            {
                img += step;
                for(j =1; j<width; j++)
                {
                    uchar* p = img + j;
                    //如果p点是背景点或者且为方向边界点，依次为北南东西，继续循环
                    if(p[0]==0||p[dir[n]]>0) continue;
                    p2 = p[-step]>0?1:0;
                    p3 = p[-step+1]>0?1:0;
                    p4 = p[1]>0?1:0;
                    p5 = p[step+1]>0?1:0;
                    p6 = p[step]>0?1:0;
                    p7 = p[step-1]>0?1:0;
                    p8 = p[-1]>0?1:0;
                    p9 = p[-step-1]>0?1:0;
                    //8 simple判定
                    int is8simple = 1;
                    if(p2==0&&p6==0)
                    {
                        if((p9==1||p8==1||p7==1)&&(p3==1||p4==1||p5==1)) 
                            is8simple = 0;
                    }
                    if(p4==0&&p8==0)
                    {
                        if((p9==1||p2==1||p3==1)&&(p5==1||p6==1||p7==1)) 
                            is8simple = 0;
                    }
                    if(p8==0&&p2==0)
                    {
                        if(p9==1&&(p3==1||p4==1||p5==1||p6==1||p7==1))
                            is8simple = 0;
                    }
                    if(p4==0&&p2==0)
                    {
                        if(p3==1&&(p5==1||p6==1||p7==1||p8==1||p9==1))
                            is8simple = 0;
                    }
                    if(p8==0&&p6==0)
                    {
                        if(p7==1&&(p3==9||p2==1||p3==1||p4==1||p5==1))
                            is8simple = 0;
                    }
                    if(p4==0&&p6==0)
                    {
                        if(p5==1&&(p7==1||p8==1||p9==1||p2==1||p3==1))
                            is8simple = 0;
                    }
                    int adjsum;
                    adjsum = p2 + p3 + p4+ p5 + p6 + p7 + p8 + p9;
                    //判断是否是邻接点或孤立点,0,1分别对于那个孤立点和端点
                    if(adjsum!=1&&adjsum!=0&&is8simple==1)
                    {
                        dst.at<uchar>(i,j) = 0; //满足删除条件，设置当前像素为0
                        ifEnd = true;
                    }

                }
            }
        }

        if(!ifEnd) break;
    }

}


string recognize(vector<Mat> subImage) //手写字模
{
	vector<Mat> CharactorLib;
	map<int,string> CharactorMap;
	vector< vector<string> > similarPair;
	for(int digit=0;digit<10;digit++)
	{
		string prePath="handWritingZiku/";
		string postPath=".png";
		string currentPath=str(digit);
		string compeletePath=prePath+currentPath+postPath;
		Mat digitImage=imread(compeletePath,0);
		threshold(digitImage,digitImage,binaryThreshold,255, CV_THRESH_BINARY_INV);
		CharactorLib.push_back(digitImage);
	}
	for(char letter='A';letter<='Z';letter++)
	{
		string prePath="handWritingZiku/";
		string postPath=".png";
		string compeletePath=prePath+letter+postPath;
		Mat letterImage=imread(compeletePath,0);
		threshold(letterImage,letterImage,binaryThreshold,255, CV_THRESH_BINARY_INV);
		CharactorLib.push_back(letterImage);
	}
	for(int i=10;i<22;i++) //省名
	{
		string prePath="handWritingZiku/";
		string postPath=".png";
		string compeletePath=prePath+str(i)+postPath;
		Mat ProvinceImage=imread(compeletePath,0);
		threshold(ProvinceImage,ProvinceImage,binaryThreshold,255, CV_THRESH_BINARY_INV);
		CharactorLib.push_back(ProvinceImage);
	}
	for(int j=0;j<CharactorLib.size();j++)
	{
		if(j<10)
			CharactorMap[j]=str(j);
		else if(j>=10 && j<36)
			CharactorMap[j]='A'+j-10;
		else
		{
			switch(j-36)
			{
				case 0:CharactorMap[j]="京";break; 
				case 1:CharactorMap[j]="津";break; 
				case 2:CharactorMap[j]="沪";break; 
				case 3:CharactorMap[j]="渝";break; 
				case 4:CharactorMap[j]="黑";break; 
				case 5:CharactorMap[j]="吉";break; 
				case 6:CharactorMap[j]="辽";break; 
				case 7:CharactorMap[j]="晋";break; 
				case 8:CharactorMap[j]="冀";break; 
				case 9:CharactorMap[j]="青";break; 
				case 10:CharactorMap[j]="豫";break; 
				case 11:CharactorMap[j]="鲁";break; 
			}
		}
	}
	int similarPairsNum=4;
	string pairs[]={"3","5","0","D","8","D","2","Z"};
	for(int i=0;i<similarPairsNum;i++)
	{
		vector<string> temp(pairs+2*i,pairs+2*i+2);
		similarPair.push_back(temp);
	}

	string plateNumber;
	for(int i=0;i<subImage.size();i++)
	{
		string character;
		double LeastError=standardWidth*standardHeight;
		double maxLikelyhood=0;
		for(int j=0;j<CharactorLib.size();j++)
		{
			double likelyhood=ImageCompare(subImage[i],CharactorLib[j],EdgeWeighted);
			//cout<<CharactorMap[j]<<" "<<error<<" "<<j<<endl;
			if(likelyhood>maxLikelyhood)
			{
				maxLikelyhood=likelyhood;
				character=CharactorMap[j];
			}
		}
		int similarIdPointer=-1;
		for(int p=0;p<similarPair.size();p++)
		{
			if(character==similarPair[p][0] || character==similarPair[p][1])
			{
				similarIdPointer=p;
				break;
			}						
		}
		if(similarIdPointer==-1);			
		else if(similarIdPointer==0)
			character=Identify_3_5(subImage[i],character);
		else if(similarIdPointer==1)
			character=Identify_0_D(subImage[i],character);
		else if(similarIdPointer==2)
			character=Identify_8_B(subImage[i],character);
		else if(similarIdPointer==3)
			character=Identify_2_Z(subImage[i],character);
		if(character=="0" || character=="O")
			character="0";
		if(character=="1" || character=="I")
			character="1";
		plateNumber+=character;
	}
	return plateNumber;
}
string Recognize(vector<Mat> subImage) //标准字模
{
	vector<Mat> CharactorLib;
	map<int,string> CharactorMap;
	vector< vector<string> > similarPair;
	int modelWidth=32;
	int modelHeight=64;
	for(int i=0;i<10;i++) //载入数字
	{
		Mat m(Size(modelWidth,modelHeight),CV_8UC1);
		for(int r=0;r<m.rows;r++)
			for(int c=0;c<m.cols;c++)
			{
				if(LPRFontGet('0'+i,c,r)==true)
					m.at<uchar>(r,c)=255;
			}
		//threshold(m,m,240,255,CV_THRESH_OTSU);
		resize(m,m,Size(standardWidth,standardHeight),0,0);
		threshold(m,m,200,255,CV_THRESH_OTSU);
		CharactorLib.push_back(m);
	}
	for(int i=0;i<26;i++) //载入数字
	{
		Mat m(Size(modelWidth,modelHeight),CV_8UC1);
		for(int r=0;r<m.rows;r++)
			for(int c=0;c<m.cols;c++)
			{
				if(LPRFontGet('A'+i,c,r)==true)
					m.at<uchar>(r,c)=255;
			}
		//threshold(m,m,240,255,CV_THRESH_OTSU);
		resize(m,m,Size(standardWidth,standardHeight),0,0);
		threshold(m,m,200,255,CV_THRESH_OTSU);
		CharactorLib.push_back(m);
		
	}
	for(int i=20;i<35;i++)
	{
		//imshow(str(i+10),CharactorLib[i]);
	}

	for(int j=0;j<CharactorLib.size();j++)
	{
		if(j<10)
			CharactorMap[j]=str(j);
		else 
			CharactorMap[j]='A'+j-10;
	}
	int similarPairsNum=4;
	string pairs[]={"3","5","0","D","8","D","2","Z"};
	for(int i=0;i<similarPairsNum;i++)
	{
		vector<string> temp(pairs+2*i,pairs+2*i+2);
		similarPair.push_back(temp);
	}

	string plateNumber;
	for(int i=0;i<subImage.size();i++)
	{
		int start=0;
		if(i==0)
			start=10;
		string character;
		double maxLikelyhood=0;
		int w=subImage[i].cols;
		int h=subImage[i].rows;
		for(int j=start;j<CharactorLib.size();j++)
		{
			//double likelyhood=ImageCompare(subImage[i],CharactorLib[j],BackgroundWeighted);			
			int CompareRound=3;
			double likelyhood=0;
			for(int r=0;r<CompareRound;r++) //滑动窗口比较
			{
				Rect CompareBox(0,0,w,h);
				CompareBox.height-=r;
				Mat crop1(subImage[i],CompareBox);
				//subImage[i]=crop1;
				CompareBox.y+=r;
				Mat crop2(CharactorLib[j],CompareBox);
				//CharactorLib[j]=crop2;
				double currentLikelyhood=ImageCompare(crop1,crop2,BackgroundWeighted);
				if(currentLikelyhood>likelyhood)
					likelyhood=currentLikelyhood;
			}
			if(likelyhood>maxLikelyhood)
			{
				maxLikelyhood=likelyhood;
				character=CharactorMap[j];
			}
		}
		int similarIdPointer=-1;
		for(int p=0;p<similarPair.size();p++)
		{
			if(character==similarPair[p][0] || character==similarPair[p][1])
			{
				similarIdPointer=p;
				break;
			}						
		}
		if(similarIdPointer==-1 || i==0);			
		else if(similarIdPointer==0)
			character=Identify_3_5(subImage[i],character);
		else if(similarIdPointer==1)
			character=Identify_0_D(subImage[i],character);
		else if(similarIdPointer==2)
			character=Identify_8_B(subImage[i],character);
		else if(similarIdPointer==3)
			character=Identify_2_Z(subImage[i],character);
		if(character=="0" || character=="O")
			character="0";
		if(character=="1" || character=="I")
			character="1";
		plateNumber+=character;
	}
	return plateNumber;
}
double ImageCompare(Mat img1,Mat img2,int distanceType)
{
	if(distanceType==NoneWeight)
	{
		double likelyhood=0;
		for(int i=0;i<img1.rows;i++)
			for(int j=0;j<img1.cols;j++)
			{
				//cout<<(int)img1.at<uchar>(i,j)<<" "<<(int)img2.at<uchar>(i,j)<<endl;
				if((int)img1.at<uchar>(i,j)==(int)img2.at<uchar>(i,j))
					likelyhood++;
			}
		return likelyhood;
	}
	else if(distanceType==EdgeWeighted)
	{
		double edgeWeight=1;
		double likelyhood=0;
		for(int i=0;i<img1.rows;i++)
			for(int j=0;j<img1.cols;j++)
			{		
				if((int)img1.at<uchar>(i,j)==255) //对于前景上的点，检测是否为边缘点
				{
					int IsEdgeOfContour=0;
					if(TestBorder(img1,i-1,j)==0 || TestBorder(img1,i+1,j)==0) //img的边缘被认为也是不重要的点
						IsEdgeOfContour=1;
					if(TestBorder(img1,i,j-1)==0 || TestBorder(img1,i,j+1)==0) 
						IsEdgeOfContour=1;
					if(IsEdgeOfContour==0)
					{
						int up=(int)img1.at<uchar>(i-1,j);
						int down=(int)img1.at<uchar>(i+1,j);
						int left=(int)img1.at<uchar>(i,j-1);
						int right=(int)img1.at<uchar>(i,j+1);
						if(up==255 && down==255 && left==255 && right==255)
							IsEdgeOfContour=0;
						else
							IsEdgeOfContour=1;
					}
					
					if(IsEdgeOfContour==0)
					{
						if((int)img1.at<uchar>(i,j)==(int)img2.at<uchar>(i,j))
						likelyhood++;
					}
					else
					{
						if((int)img1.at<uchar>(i,j)==(int)img2.at<uchar>(i,j))
						likelyhood+=edgeWeight*1;
					}
				}
				else
				{
					if((int)img1.at<uchar>(i,j)==(int)img2.at<uchar>(i,j))
					likelyhood++;
				}
			}
			return likelyhood;
	}
	else if(distanceType==BackgroundWeighted)
	{
		double likelyhood=0;
		double BackgroundWeight=0.8;
		for(int i=0;i<img1.rows;i++)
			for(int j=0;j<img1.cols;j++)
			{
				//cout<<(int)img1.at<uchar>(i,j)<<" "<<(int)img2.at<uchar>(i,j)<<endl;
				if((int)img1.at<uchar>(i,j)==(int)img2.at<uchar>(i,j))
				{
					if((int)img1.at<uchar>(i,j)==255)
						likelyhood++;
					else
						likelyhood+=BackgroundWeight;
				}
			}
			return likelyhood;
	}
}
string Identify_3_5(Mat img,string firststep) //图像分成四块，左上角的像素点个数比右上角多，则认为是5，否则是3
{
	int w=img.cols;
	int h=img.rows;
	int topLeftPixNum=0;
	int topRightPixNum=0;
	int oneside=w/2; //解决宽度奇数偶数的问题
	for(int i=0;i<h/2;i++)
		for(int j=0;j<oneside;j++)
			if((int)img.at<uchar>(i,j)==255)
				topLeftPixNum++;
	for(int i=0;i<h/2;i++)
		for(int j=w-1;j>w-1-oneside;j--)
			if((int)img.at<uchar>(i,j)==255)
				topRightPixNum++;
	if(topLeftPixNum>topRightPixNum)
		return "5";
	else if(topLeftPixNum<topRightPixNum)
		return "3";
	else
		return firststep;
}
string Identify_0_D(Mat img,string firststep) //取图像四个角3*3的小矩形，以此区别0和D
{
	int w=img.cols;
	int h=img.rows;
	int boxsize=2;
	int topLeftPixNum=0;
	int topRightPixNum=0;
	int bottomLeftPixNum=0;
	int bottomRightPixNum=0;
	for(int i=0;i<boxsize;i++)
		for(int j=0;j<boxsize;j++)
			if((int)img.at<uchar>(i,j)==255)
				topLeftPixNum++;
	for(int i=0;i<boxsize;i++)
		for(int j=w-1;j>w-1-boxsize;j--)
			if((int)img.at<uchar>(i,j)==255)
				topRightPixNum++;
	for(int i=h-1;i>h-1-boxsize;i--)
		for(int j=0;j<boxsize;j++)
			if((int)img.at<uchar>(i,j)==255)
				bottomLeftPixNum++;
	for(int i=h-1;i>h-1-boxsize;i--)
		for(int j=w-1;j>w-1-boxsize;j--)
			if((int)img.at<uchar>(i,j)==255)
				bottomRightPixNum++;

	if(topLeftPixNum+bottomLeftPixNum-topRightPixNum-bottomRightPixNum>=4)
		return "D";
	else
		return "0";
}
string Identify_8_B(Mat img,string firststep) //取图像四个角3*3的小矩形，以此区别0和D
{
	int w=img.cols;
	int h=img.rows;
	int boxsize=2;
	int topLeftPixNum=0;
	int topRightPixNum=0;
	int bottomLeftPixNum=0;
	int bottomRightPixNum=0;
	for(int i=0;i<boxsize;i++)
		for(int j=0;j<boxsize;j++)
			if((int)img.at<uchar>(i,j)==255)
				topLeftPixNum++;
	for(int i=0;i<boxsize;i++)
		for(int j=w-1;j>w-1-boxsize;j--)
			if((int)img.at<uchar>(i,j)==255)
				topRightPixNum++;
	for(int i=h-1;i>h-1-boxsize;i--)
		for(int j=0;j<boxsize;j++)
			if((int)img.at<uchar>(i,j)==255)
				bottomLeftPixNum++;
	for(int i=h-1;i>h-1-boxsize;i--)
		for(int j=w-1;j>w-1-boxsize;j--)
			if((int)img.at<uchar>(i,j)==255)
				bottomRightPixNum++;

	if(topLeftPixNum+bottomLeftPixNum-topRightPixNum-bottomRightPixNum>=4)
		return "B";
	else
		return "8";
}
string Identify_2_Z(Mat img,string firststep) //只对图像的上半部分进行匹配（或者还是用检测角落的办法）
{
	int w=img.cols;
	int h=img.rows;
	int boxsize=2;
	int topLeftPixNum=0;
	int topRightPixNum=0;
	int bottomLeftPixNum=0;
	int bottomRightPixNum=0;
	for(int i=0;i<boxsize;i++)
		for(int j=0;j<boxsize;j++)
			if((int)img.at<uchar>(i,j)==255)
				topLeftPixNum++;
	for(int i=0;i<boxsize;i++)
		for(int j=w-1;j>w-1-boxsize;j--)
			if((int)img.at<uchar>(i,j)==255)
				topRightPixNum++;

	if(topLeftPixNum+topRightPixNum>=4)
		return "Z";
	else
		return "2";
}
