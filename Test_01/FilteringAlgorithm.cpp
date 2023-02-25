#include "Include.h"

Filtering::Filtering(QString s) {
	type = s;
	dataManager = new DataManager();
}

Filtering::~Filtering(){}

void Filtering::Smooth(DataManager* data) {
	dataManager->oriData = (Data*)calloc(data->NumberOfData, sizeof(Data));
	dataManager->NumberOfData = data->NumberOfData - 4;
	dataManager->filename = data->filename;

	for (int i = 2; i < (data->NumberOfData - 2); i++)
	{
		dataManager->oriData[i - 2].x = data->oriData[i].x;
		dataManager->oriData[i - 2].y = (-3 * data->oriData[i - 2].y + 12 * data->oriData[i - 1].y + 17 * data->oriData[i].y +
			12 * data->oriData[i + 1].y - 3 * data->oriData[i + 2].y) / 35;
	}

	return;
}

int Filtering::LimitedAmplitudeFiltering(DataManager* data) {
	double variationRange = VARIATION_RANGE * data->Factor;
	//根据源数据的数量级重新计算VARIATION_RANGE

	double temp = 0;
	double B = 0;
	double start = data->oriData[0].y;

	dataManager->oriData = (Data*)calloc(data->NumberOfData, sizeof(Data));
	dataManager->NumberOfData = data->NumberOfData;
	dataManager->filename = data->filename;

	for (int i = 0; i < data->NumberOfData; i++)
	{
		dataManager->oriData[i].x = data->oriData[i].x;
		B = start - data->oriData[i].y;
		if ( fabs(B) > variationRange && i != 0)
			dataManager->oriData[i].y = dataManager->oriData[i-1].y;
		else
			dataManager->oriData[i].y = data->oriData[i].y;
	}
	return 0;
}