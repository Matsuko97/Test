#pragma once

#define iPeak_No 3
#define RoundRatio 100

//Peak-Finding Algorithm ******** Symmetric Zero Area
//Gaussian
static int m = 5;//2m+1=W
static int W = 11;
static int H = 4;
static double f = 0.5;

//Square
//static int m = 31;//2m+1=W
//static int W = 63;
//static int H = 21;
//static double f = 2.0;
//End ******** Symmetric Zero Area

struct PeakNode {
	int indPeak;
	int indStart;
	int indEnd;
	int indWidth;
	struct PeakNode* next;
};

class PeakFinding {
public:
	PeakNode* Peaks;
	PeakNode* Rear;
	QString type;
	QString fileName;
	QString fileNameBase;

public:
	PeakFinding(QString s);
	~PeakFinding();

	//Peak-Finding Algorithm ******** Trend Accumulation
	void TrendAccumulation(DataManager* data);
	void FindPeaks(int* src, int ReadNums);
	int ScreenPeaks(int* src);
	void FreeLink(PeakNode* p);

	//Peak-Finding Algorithm ******** Symmetric Zero Area
	double SZA_G(int j);
	double SZA_C(int j);
	int SymmetricZeroArea(DataManager* data);

	//BaseLine ******** SNIP
	void BasicSNIP(DataManager* data, int num, int m, int i, bool type, QString str);
	void ImprovedSNIP(DataManager* data, PeakNode* Head);
	void ObtainInfo(PeakNode* p, int& start, int& end, int& width, int num);

	QString GenerateFileName(QString filename, QString type);
	bool WriteData(QString filename, QString str);
	bool WriteData(QString filename, PeakNode* Peaks, int* TestData, int PeaksThreshold, DataManager* data);
};