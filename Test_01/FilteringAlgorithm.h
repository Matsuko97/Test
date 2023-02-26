#pragma once

#define AverageNum      31
#define DebounceNum     10
#define VARIATION_RANGE 0.2
#define RecursiveNum    31

struct Platform{
	int           indStart;
	int           indEnd;
	int           indWidth;
	struct Platform* next;
	Platform(int x) : indStart(x), next(nullptr) {}
};

class Filtering {
public:
	QString type;
	DataManager* dataManager;

public:
	Filtering(QString s);
	~Filtering();

	int AverageFilter(DataManager* data);
	int DebounceFilter(DataManager* data);
	int LimitedAmplitudeFilter(DataManager* data);
	int RecursiveMedianFilter(DataManager* data);
	void LeastSquares(Data* data, int num, double* k, double* b);
	void Smooth(DataManager* data);
};