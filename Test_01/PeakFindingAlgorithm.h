#pragma once

#define iPeak_No 3
#define RoundRatio 100

struct PeakNode {
	int           indPeak;
	int           indStart;
	int           indEnd;
	struct PeakNode* next;
};

class PeakFinding {
public:
	PeakNode* Peaks;

public:
	//Peak-Finding Algorithm ******** Trend Accumulation
	void TrendAccumulation(DataManager* data);
	void FindPeaks(int* src, int ReadNums);
	int ScreenPeaks(int* src);
	void FreeLink(PeakNode* p);


};