#include "calib.h"

//stringתfloat���͵ĺ���
template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}
//��ȡ�ļ�
void getFilesName(string& File_Directory, string& FileType, vector<string>& FilesName, vector<string>& FilesName2)
{

	string buffer = File_Directory + "\\*";

	_finddata_t c_file;   // ����ļ����Ľṹ��

	intptr_t hFile;
	hFile = _findfirst(buffer.c_str(), &c_file);   //�ҵ�һ���ļ���

	if (hFile == -1L)   // ����ļ���Ŀ¼�´�����Ҫ���ҵ��ļ�
		printf("No %s files in current directory!\n", FileType);
	else
	{
		string fullFilePath;
		string m_fullFilePath;
		string m2_fullFullPath;
		do
		{
			fullFilePath.clear();

			//����
			string tmp_dir = ".";
			string last_dir = "..";
			if (c_file.name == tmp_dir || c_file.name == last_dir)
				continue;
			fullFilePath = File_Directory + "\\" + c_file.name;
			_finddata_t m_file;
			intptr_t m_hFile;
			string tmpFullFillPath = fullFilePath + "\\*" + FileType;
			m_hFile = _findfirst(tmpFullFillPath.c_str(), &m_file);

			if (m_hFile == -1L)
			{
				std::cout << tmpFullFillPath << "�ļ��������ļ�" << endl;
			}
			else
			{
				m_fullFilePath.clear();
				m_fullFilePath = fullFilePath + "\\" + m_file.name;
				auto flag = _findnext(m_hFile, &m_file);
				m2_fullFullPath = fullFilePath + "\\" + m_file.name;
			}
			FilesName.push_back(m_fullFilePath);
			FilesName2.push_back(m2_fullFullPath);
			_findclose(m_hFile);
		} while (_findnext(hFile, &c_file) == 0);  //����ҵ��¸��ļ������ֳɹ��Ļ��ͷ���0,���򷵻�-1  
		_findclose(hFile);
	}
}

void get_peak(vector<string>& FileName, vector<string>& FileName2, vector<Mat>& picMat, int model)
{
	ifstream inFile;
	ifstream inFile2;
	string oriData;
	string oriData2;
	string Data;
	string Data2;
	float tmpData, tmpData2, myTmpData;
	vector<float> myData;
	vector<float> myData2;
	Mat img;
	int pic_row = 64;
	int pic_col = 256;
	int data_cnt = 16386;
	if (model == MODEL_4310)
	{
		pic_row = 120;
		pic_col = 160;
		data_cnt = 19202;
	}
	else if (model == MODEL_4320)
	{
		pic_row = 64;
		pic_col = 256;
		data_cnt = 16386;
	}
	for (int i = 0; i < FileName.size(); i++)
	{
		inFile.open(FileName[i]);
		inFile2.open(FileName2[i]);
		assert(inFile.is_open());
		assert(inFile2.is_open());
		myData.clear();
		int cnt_cols = 0;
		int pic_num = 0;
		size_t pos1;
		size_t pos2;
		double minv = 0.0, maxv = 0.0;
		double* minp = &minv;
		double* maxp = &maxv;
		pic_num++;
		int tmp = 0;
		while (getline(inFile, oriData))
		{
			getline(inFile2, oriData2);
			cnt_cols++;
			if (cnt_cols > 1 && cnt_cols < data_cnt)
			{
				pos1 = oriData.find(' ');
				pos2 = oriData.length();
				Data = oriData.substr(pos1 + 1, pos2);
				tmpData = stringToNum<float>(Data);
				pos1 = oriData2.find(' ');
				pos2 = oriData2.length();
				Data2 = oriData2.substr(pos1 + 1, pos2);
				tmpData2 = stringToNum<float>(Data2);
				tmpData = (tmpData + tmpData2) / 2;
				myData.push_back(tmpData);//�����ݴ���mat������	
			}
		}

		img = Mat(myData, true);
		inFile.close();
		inFile2.close();
		img = img.reshape(1, pic_row);//��ͼƬתΪ��120��160��
		if (img.cols == pic_col)
		{
			minMaxIdx(img, minp, maxp);
			for (int col_t = 0; col_t < img.cols; col_t++)
				for (int row_t = 0; row_t < img.rows; row_t++)
				{
					float& img_pixel = img.at<float>(row_t, col_t);
					img_pixel = (img_pixel / *maxp) * 255;
				}

			string name;
			Mat img_n;
			img.convertTo(img_n, CV_8UC1);//��ͼ��תΪuchar��������
			picMat.push_back(img_n);
		}
		else
		{
			std::cout << FileName[i] << "ͼ������" << endl;
			continue;
		}
		inFile.clear();

	}

}


//��ȡ������
bool m_findchessbord(vector<Mat>& picMat, const Size board_size, const Size square_size, vector<vector<Point2f>>& image_points_seq, Size& image_size)
{
	int image_count = 0;                                            // ͼ������
	int cnt = 0;
	string image_count_tmp;
	stringstream Strcnt;
	vector<Point2f> image_points;                                   // ����ÿ��ͼ���ϼ�⵽�Ľǵ�
	for (int i = 0; i < picMat.size(); i++)
	{
		image_count++;

		// ���ڹ۲�������
		Mat img = picMat[i];
		Mat imageInput;
		Mat thresholdPic;
		img.convertTo(imageInput, CV_8UC1);
		if (image_count == 1)  //�����һ��ͼƬʱ��ȡͼ������Ϣ
		{
			image_size.width = imageInput.cols;
			image_size.height = imageInput.rows;
			std::cout << "image_size.width = " << image_size.width << endl;
			std::cout << "image_size.height = " << image_size.height << endl;
		}

		/* ��ȡ�ǵ� */
		bool patternfound = findChessboardCorners(imageInput, board_size, image_points, CALIB_CB_ADAPTIVE_THRESH);
		if (!patternfound)
		{
			std::cout << "��" << image_count << "����Ƭ��ȡ�ǵ�ʧ�ܣ�" << endl; //�Ҳ����ǵ�
			continue;
		}
		image_points_seq.push_back(image_points);						//���������ؽǵ�
	}
	std::cout << "�ǵ���ȡ��ɣ�����" << endl;
	return true;
}
//������������б궨
bool m_caibration(vector<vector<Point2f>>& image_points_seq, const Size board_size, const Size square_size, const Size image_size, Mat& cameraMatrix, Mat& distCoeffs, vector<Mat>& rvecsMat, vector<Mat>& tvecsMat, int model)
{
	/*������ά��Ϣ*/
	vector<vector<Point3f>> object_points_seq;                     // ����궨���Ͻǵ����ά����
	int pic_succeed = image_points_seq.size();
	for (int t = 0; t < pic_succeed; t++)
	{
		vector<Point3f> object_points;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				Point3f realPoint;
				/* ����궨�������������ϵ��z=0��ƽ���� */
				realPoint.x = i * square_size.width;
				realPoint.y = j * square_size.height;
				realPoint.z = 0;
				object_points.push_back(realPoint);
			}
		}
		object_points_seq.push_back(object_points);
	}
	if (pic_succeed > 2)
	{
		double err_first = 0;
		if (model == MODEL_4310)
			err_first = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, CALIB_ZERO_TANGENT_DIST);
		else if (model == MODEL_4320)
			err_first = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, CALIB_FIX_K3 | CALIB_ZERO_TANGENT_DIST);
		//double err_first = calibrateCamera(object_points_seq, image_points_seq, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, CALIB_RATIONAL_MODEL | CALIB_ZERO_TANGENT_DIST| CALIB_FIX_K5| CALIB_FIX_K6);
		ofstream res_out("result.txt");                       // ����궨������ļ�
		double fx = cameraMatrix.at<double>(0, 0);
		double fy = cameraMatrix.at<double>(1, 1);
		double cx = cameraMatrix.at<double>(0, 2);
		double cy = cameraMatrix.at<double>(1, 2);
		double k1 = distCoeffs.at<double>(0);
		double k2 = distCoeffs.at<double>(1);
		double p1 = distCoeffs.at<double>(2);
		double p2 = distCoeffs.at<double>(3);
		double k3 = distCoeffs.at<double>(4);
		//double k4 = distCoeffs.at<double>(5);
		res_out << fx << endl;
		res_out << fy << endl;
		res_out << cx << endl;
		res_out << cy << endl;
		res_out << k1 << endl;
		res_out << k2 << endl;
		res_out << p1 << endl;
		res_out << p2 << endl;
		res_out << k3 << endl;
		std::cout << "fx,fy,cx,cy,k1,k2,p1,p2�������" << endl;
		res_out << endl;
		return true;
	}
	else
		cout << "ʶ����ı궨��ͼ����3��" << endl;
	return 1;
}

//�궨
bool calib(string& File_Directory1, string FileType = ".txt", const Size board_size = Size(4, 5), const Size square_size = Size(50, 50), bool is_undistort = false, int model = MODEL_4320)
{
	vector<string>FilesName1, FilesName2;								// ����ļ���������
	vector<Mat>picMat;
	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));				// ������ڲ�������
	Mat distCoeffs;														// �������5������ϵ����k1,k2,p1,p2,k3
	vector<Mat> rvecsMat;												// �������ͼ�����ת������ÿһ��ͼ�����ת����Ϊһ��mat
	vector<Mat> tvecsMat;												// ƽ������
	getFilesName(File_Directory1, FileType, FilesName1, FilesName2);	// �궨����ͼ���ļ���·��
	get_peak(FilesName1, FilesName2, picMat, model);					// ����ǿtxtתΪ�Ҷ�ͼƬ
	
	vector<vector<Point2f>> image_points_seq;
	Size image_size;


	if (!m_findchessbord(picMat, board_size, square_size, image_points_seq, image_size))
		std::cout << "�ǵ���ȡʶ��ʧ��" << endl;
	if (!m_caibration(image_points_seq, board_size, square_size, image_size, cameraMatrix, distCoeffs, rvecsMat, tvecsMat, model))
		std::cout << "�궨ʧ��" << endl;
	return true;
}


int main()
{
	int model = MODEL_4320;											// ģ������
	string File_Directory1 = "D:\\calibra\\4320\\20200629_4x5";		// �ļ���Ŀ¼1
	//string File_Directory1 = "D:\\calibra\\4310\\0702_4x5_50"; 
	string FileType = ".txt";										// ��Ҫ���ҵ��ļ�����
	calib(File_Directory1, FileType, Size(5, 4), Size(50, 50), false, model);

	return 0;
}