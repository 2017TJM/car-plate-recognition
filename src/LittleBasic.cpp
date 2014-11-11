#include "stdafx.h"
#include "LittleBasic.h"

string str(int i)
{
	stringstream ss;
	ss<<i;
	string s;
	ss>>s;
	return s;
}
