#include "Include.h"

Filtering::Filtering(QString s) {
	type = s;
	dataManager = new DataManager();
}

Filtering::~Filtering(){
	delete dataManager;
	dataManager = nullptr;
}

int Filtering::AverageFilter(DataManager* data) {
	if (data == nullptr){
		return -1;
	}

	int i = 0, Num = data->NumberOfData;
	double result = 0;
	double* dataTemp;
	dataTemp = (double*)calloc(Num, sizeof(double));

	dataManager->oriData = (Data*)calloc(Num, sizeof(Data));
	dataManager->NumberOfData = Num - AverageNum;
	dataManager->filename = data->filename;

	for (int a = 0; a < Num - AverageNum; a = a + AverageNum)
	{
		for (i = a; i < (AverageNum + a); i++)
			dataTemp[i - a] = data->oriData[i].y;

		result = 0;
		for (int count = 0; count < AverageNum; count++)
			result += dataTemp[count];

		result = result / AverageNum;

		for (i = a; i < (AverageNum + a); i++)
		{
			dataManager->oriData[i].x = data->oriData[i].x;
			dataManager->oriData[i].y = result;
		}
		memset(dataTemp, 0, sizeof(double) * AverageNum);
	}

	free(dataTemp);
	dataTemp = NULL;

	return 0;
}

int Filtering::DebounceFilter(DataManager* data) {
	if (data == nullptr) {
		return -1;
	}

	int n = 0, Num = data->NumberOfData;
	double temp = data->oriData[0].y;

	dataManager->oriData = (Data*)calloc(Num, sizeof(Data));
	dataManager->NumberOfData = Num;
	dataManager->filename = data->filename;

	for (int i = 0; i < Num; i++)
	{
		dataManager->oriData[i].x = data->oriData[i].x;
		if (temp != (data->oriData[i].y))
		{
			n++;
			if (n > DebounceNum)
			{
				n = 0;
				temp = data->oriData[i].y;
			}
			dataManager->oriData[i].y = temp;
		}
		else
		{
			n = 0;
			dataManager->oriData[i].y = temp;
		}
	}

	return 0;
}

int Filtering::LimitedAmplitudeFilter(DataManager* data) {
	if (data == nullptr) {
		return -1;
	}

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

int Filtering::RecursiveMedianFilter(DataManager* data) {
	if (data == nullptr) {
		return -1;
	}

	int Num = data->NumberOfData;

	double result = 0;
	double change;
	double k, b;
	bool Flag = true;

	double* dataTemp;
	Data* dataExtension;

	int i, j, nCount = RecursiveNum / 2;

	double temp = 0;

	dataTemp = (double*)malloc(sizeof(double) * RecursiveNum);

	dataExtension = (Data*)malloc(sizeof(Data) * RecursiveNum);

	dataManager->oriData = (Data*)calloc(Num, sizeof(Data));
	dataManager->NumberOfData = Num;
	dataManager->filename = data->filename;

	for (int a = 0; a < (Num - 2 * nCount); a++)
	{
		for (i = a; i < (RecursiveNum + a); i++)
			dataTemp[i - a] = (data->oriData[i].y);

		for (i = 1; i < RecursiveNum; i++)
			for (j = 0; j < RecursiveNum - i; j++)
			{
				if (dataTemp[j] > dataTemp[j + 1])
				{
					change = dataTemp[j];
					dataTemp[j] = dataTemp[j + 1];
					dataTemp[j + 1] = change;
				}
			}

		result = dataTemp[nCount];

		if (0 == a)
		{
			for (i = 0; i < RecursiveNum; i++)
			{
				dataExtension[i].x = data->oriData[i].x;
				dataExtension[i].y = data->oriData[i].y;
			}
			LeastSquares(dataExtension, RecursiveNum, &k, &b);
			for (i = 0; i < nCount; i++)
			{
				dataManager->oriData[i].x = data->oriData[i].x;
				dataManager->oriData[i].y = k * data->oriData[i].x + b;
			}
			memset(dataExtension, 0, sizeof(Data) * RecursiveNum);
		}

		dataManager->oriData[a + nCount].x = data->oriData[a + nCount].x;
		dataManager->oriData[a + nCount].y = result;
		memset(dataTemp, 0, sizeof(double) * RecursiveNum);
	}

	free(dataExtension);
	dataExtension = (Data*)malloc(sizeof(Data) * nCount);

	for (i = 0; i < nCount; i++)
	{
		dataExtension[i].x = dataManager->oriData[Num - RecursiveNum + i].x;
		dataExtension[i].y = dataManager->oriData[Num - RecursiveNum + i].y;
	}
	LeastSquares(dataExtension, nCount, &k, &b);
	for (i = Num - nCount; i < Num; i++)
	{
		dataManager->oriData[i].x = data->oriData[i].x;
		dataManager->oriData[i].y = k * data->oriData[i].x + b;
	}
	memset(dataExtension, 0, sizeof(Data) * nCount);

	int count = 1, start = 0;
	Platform* HeadNode = new Platform(-1);
	Platform* p = HeadNode, * q = nullptr;
	for (int n = 1; n < Num; n++) {
		if (dataManager->oriData[n].y == dataManager->oriData[n - 1].y) {
			if (count >= 7) {
				q = new Platform(start);
			}
			else {
				start = start == 0 ? n - 1 : start;
			}
			count = start == 0 ? 1 : ++count;
		}
		else {
			if (q) {
				q->indEnd = n-1;
				q->indWidth = count;
				p->next = q;
				p = q;
				q = nullptr;

				start = 0;
				count = 1;
			}
		}
	}

	p = HeadNode->next;
	while (!p)
	{
		int n = p->indStart;
		int a = 0;
		int listlength = p->indWidth;
		double* Temp = (double*)calloc(listlength, sizeof(double));
		for (; listlength >= 0; listlength = listlength - 2, n++)
		{
			for (i = n - listlength / 2; i <= n + listlength / 2; a++, i++)
				Temp[a] = data->oriData[i].y;
			for (i = 1; i < listlength; i++)
				for (j = 0; j < listlength - i; j++)
				{
					if (Temp[j] > Temp[j + 1])
					{
						change = Temp[j];
						Temp[j] = Temp[j + 1];
						Temp[j + 1] = change;
					}
				}
			result = Temp[listlength / 2];
			dataManager->oriData[n].y = result;
		}

		n = p->indEnd;
		for (; listlength > 1; listlength = listlength - 2, n--)
		{
			for (i = n - listlength / 2; i <= n + listlength / 2; a++, i++)
				Temp[a] = data->oriData[i].y;
			for (i = 1; i < listlength; i++)
				for (j = 0; j < listlength - i; j++)
				{
					if (Temp[j] > Temp[j + 1])
					{
						change = Temp[j];
						Temp[j] = Temp[j + 1];
						Temp[j + 1] = change;
					}
				}

			result = Temp[nCount];
			dataManager->oriData[n].y = result;
		}
		dataManager->oriData[n - 1].y = data->oriData[n - 1].y;
		p = p->next;
		
		memset(Temp, 0, p->indWidth * sizeof(Temp));
	}
	delete HeadNode;
	delete p;
	p = nullptr;
	HeadNode = nullptr;

	free(dataTemp);
	free(dataExtension);
	dataTemp = NULL;
	dataExtension = NULL;

	return 0;
}

void Filtering::LeastSquares(Data* data, int num, double* k, double* b) {
	double xSquareSum = 0.0;
	double ySum = 0.0;
	double xSum = 0.0;
	double XY = 0.0;
	double xSumAverage = 0.0;
	double ySumAverage = 0.0;

	for (int i = 0; i < num; ++i)
	{
		xSquareSum += data[i].x * data[i].x;
		ySum += data[i].y;
		xSum += data[i].x;
		XY += data[i].x * data[i].y;
	}

	xSumAverage = xSum / num;
	ySumAverage = ySum / num;

	*k = (XY / num - xSumAverage * ySumAverage) / (xSquareSum / num - xSumAverage * xSumAverage);
	*b = (xSquareSum * ySum - xSum * XY) / (num * xSquareSum - xSum * xSum);

	return;
}

void Filtering::Smooth(DataManager* data) {
	if (data == nullptr) {
		return;
	}

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