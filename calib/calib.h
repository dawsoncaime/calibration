#ifndef _CALIB_H_
#define _CALIB_H_

#include "opencv2\opencv.hpp"
#include <iostream>
#include <fstream>
#include <iostream>  
#include<io.h>
#include <string>  
#include<vector>
#include <sstream>
#include<direct.h>
#include<cmath>

using namespace cv;
using namespace std;

#define MODEL_4310 0
#define MODEL_4320 1

#endif
template <class Type>
Type stringToNum(const string&);
//读取数据
void getFilesName(string&, string&, vector<string>&, vector<string>&);
void get_peak(vector<string>&, vector<string>&, vector<Mat>&, int);
//标定
bool m_findchessbord(vector<Mat>&, const Size, const Size, vector<vector<Point2f>>&, Size&);
bool m_caibration(vector<vector<Point2f>>&, const Size, const Size, const Size, Mat&, Mat&, vector<Mat>&, vector<Mat>&, int);
bool calib(string&, string, const Size, const Size, bool, int);
