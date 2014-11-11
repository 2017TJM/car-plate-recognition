#ifndef LITTLEBASIC_H
#define LITTLEBASIC_H

#include "core/core.hpp"
#include "highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <math.h>
#include <queue>
#include <stack>
using namespace std;
using namespace cv;

string str(int i);
template <typename T> T vector_max_value(vector<T> v)
{
	if (v.size()==0)
		return 0;
	else
	{
		T max=v[0];
		for(int i=0;i<v.size();i++)
			if(v[i]>max)
				max=v[i];
		return max;
	}
}
template <typename T> T vector_min_value(vector<T> v)
{
	if (v.size()==0)
		return 0;
	else
	{
		T min=v[0];
		for(int i=0;i<v.size();i++)
			if(v[i]<min)
				min=v[i];
		return min;
	}
}
template <typename T> void delete_value_x(vector<T> & v,T x)
{
	for(vector<T>::iterator it = v.begin();it!=v.end();)
	{
		if(*it==x)
			it=v.erase(it); //erase后指针失效，需要重新赋值，写成it=v.erase(it，it+1)也是错的，因为返回值需要用到last_pos，此时的it+1已失效;
		else
			it++;
	}
}
template <typename T> void delete_pos_i(vector<T> & v,int i)
{
	vector<T>::iterator it = v.begin()+i;
	v.erase(it);
}
template <typename T> T odd(T x)
{
	x=floor((double)x);
	if((int)x%2==1)
		return x;
	else
		return x-1;
}

#endif
