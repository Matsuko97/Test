#pragma once

#define VARIATION_RANGE 0.2

class Filtering {
public:
	QString type;
	DataManager* dataManager;

public:
	Filtering(QString s);
	~Filtering();

	void Smooth(DataManager* data);
	int LimitedAmplitudeFiltering(DataManager* data);
};