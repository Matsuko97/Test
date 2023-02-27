#include "Include.h"

PeakFinding::PeakFinding(QString s) {
	Peaks = nullptr;
	Rear = nullptr;
	type = s;
	fileName = "";
	fileNameBase = "";
}

PeakFinding::~PeakFinding(){}

void PeakFinding::TrendAccumulation(DataManager* data) {
	Data* oriData = data->GetOriData();
	int num = data->NumberOfData;
	int j, PeaksThreshold = 0;
	int* TestData = (int*)calloc(data->NumberOfData, sizeof(int));
	PeakNode* p;

	for (j = 0; j < num; j++)
	{
		TestData[j] = oriData[j].y * RoundRatio;
	}

	FindPeaks(TestData, num);

	if (Peaks == NULL) //片段中无峰
	{
	}

	PeaksThreshold = ScreenPeaks(TestData);
	
	fileName = GenerateFileName(data->filename, "Trend_Accumulation");
	WriteData(fileName, Peaks, TestData, PeaksThreshold, data);

	FreeLink(Peaks);
	Peaks = nullptr;
}

void PeakFinding::FindPeaks(int* src, int ReadNums)
{
	int i, j, diff, accumulation = 0, peakThreshold = 0, count = 0, maxDiff = 0;
	int* sign = (int*)calloc(ReadNums, sizeof(int));
	PeakNode* p;
	PeakNode* q;
	bool        Flag = true;

	for (i = 1; i < ReadNums; i++)
	{
		diff = src[i] - src[i - 1];
		if (diff > 0)          sign[i - 1] = 1;
		else if (diff < 0)     sign[i - 1] = -1;
		else                   sign[i - 1] = 0;
	}

	for (i = 0; i < ReadNums - 1; i++)
	{
		accumulation += sign[i];

		if (accumulation > 0)              //>判断波峰
		{
			if (count == 0)            //判断是否是新的波峰
			{
				if (Flag)              //判断头节点是否已经建立
				{
					Peaks = (PeakNode*)malloc(sizeof(PeakNode));
					Peaks->next = NULL;
					p = Peaks;
					Flag = false;
				}
				else
				{
					q = (PeakNode*)malloc(sizeof(PeakNode));
					q->next = NULL;
					p->next = q;
					p = q;
				}
				p->indStart = i;       //波峰开始点记录
				p->indPeak = i + 1;    //目前的峰值点记录

			}
			p->indEnd = i + 1;        //目前的峰尾点记录
			if (src[i + 1] > peakThreshold)    //寻找峰值点,用实际的值进行
			{
				peakThreshold = src[i + 1];
				p->indPeak = i + 1;

			}
			count++;               //记录组成波峰的点数
		}

		if (accumulation < 0)         //判断波谷
		{
			if (count == 0)
			{
				if (Flag)
				{
					Peaks = (PeakNode*)malloc(sizeof(PeakNode));
					Peaks->next = NULL;
					p = Peaks;
					Flag = false;
				}
				else
				{
					q = (PeakNode*)malloc(sizeof(PeakNode));
					q->next = NULL;
					p->next = q;
					p = q;
				}
				p->indStart = i;
				peakThreshold = src[i];
				p->indPeak = i + 1;

			}
			p->indEnd = i + 1;
			if (src[i + 1] < peakThreshold)
			{
				peakThreshold = src[i + 1];
				p->indPeak = i + 1;
			}
			count++;

		}

		if (accumulation == 0 && count != 0)     //波峰波谷尾部判断
		{
			p->indEnd = i + 1;
			count = 0;
			peakThreshold = 0;
		}

		//对于波峰或波谷已到达尾部，但加和不为零的情况
		if (count > 500)                    //若波峰或者波谷数据超过一定量，这里设500
		{
			if (abs(src[p->indEnd] - peakThreshold) <= RoundRatio / 10 && abs(peakThreshold - src[p->indStart]) >= RoundRatio * 80 / 100)//当目峰的数据数量已经达到500，但实际峰还未走到需要确定尾部的地方且此峰为有效峰时，不进行尾部判断
			{
				count = count - 100;
			}
			else
			{
				maxDiff = 0;
				diff = 0;
				for (j = i; j < i + 100; j++)    //计算501点后100点每个点与501点的差值，取最大值
				{
					diff = src[j] - src[i];
					if (abs(diff) > maxDiff)
					{
						maxDiff = abs(diff);
					}
				}

				if (maxDiff <= RoundRatio / 10)   //若最大值小于设定的阈值则认为已经达到尾部
				{
					p->indEnd = i + 1;
					count = 0;
					peakThreshold = 0;
					accumulation = 0;  //对于波峰或波谷已到达尾部，但加和不为0的情况需将加和归零
				}
				else
				{
					count = count - 100;
				}
			}

		}

	}

	p = NULL;
	q = NULL;
}

int PeakFinding::ScreenPeaks(int* src)   // 遍历检测到的峰，取峰值点第三大的峰高作为阈值
{
	PeakNode*    p;
	int* iPeaksHeight = (int*)malloc(iPeak_No * sizeof(int));
	int  i, PeaksThreshold;

	i = iPeak_No - 1;

	while (i >= 0)
	{
		*(iPeaksHeight + i) = 0;
		p = Peaks;
		while (p != NULL)
		{
			if (i == iPeak_No - 1)
			{
				if (abs(src[p->indPeak] - src[p->indStart]) > *(iPeaksHeight + i))
				{
					*(iPeaksHeight + i) = abs(src[p->indPeak] - src[p->indStart]);
				}
				p = p->next;
			}
			else
			{
				if (abs(src[p->indPeak] - src[p->indStart]) < *(iPeaksHeight + i + 1))
				{
					if (abs(src[p->indPeak] - src[p->indStart]) > *(iPeaksHeight + i))
					{
						*(iPeaksHeight + i) = abs(src[p->indPeak] - src[p->indStart]);
					}

				}
				p = p->next;
			}
		}
		i--;
	}

	p = NULL;
	PeaksThreshold = *(iPeaksHeight);

	free(iPeaksHeight);
	iPeaksHeight = NULL;

	return PeaksThreshold;
}

void PeakFinding::FreeLink(PeakNode* p)
{
	PeakNode* q;
	while (p != NULL)
	{
		q = p->next;
		free(p);
		p = q;
	}
	p = NULL;
}

//Peak-Finding Algorithm ******** Symmetric Zero Area
double PeakFinding::SZA_G(int j) {
	double result = 0;

	result = exp(-4 * (j * j / H / H) * log(2.0));
	return result;
}

double PeakFinding::SZA_C(int j) {
	//Gaussian
#if 1
	double result = 0;

	for (int i = -m; i <= m; ++i)
	{
		result += SZA_G(i);
	}

	result = result / W;
	result = SZA_G(j) - result;
	return result;
#endif

	//Square Wave
#if 0
	double result = 0;

	if (j >= ((1 - H) / 2) && j <= ((H - 1) / 2))
	{
		result = 2;
	}
	else
	{
		result = -1;
	}
	return result;
#endif
}

int PeakFinding::SymmetricZeroArea(DataManager* data) {
	if (data == nullptr){
		return -1;
	}

	int num = data->NumberOfData;
	fileName = GenerateFileName(data->filename, "Symmetric_Zero_Area");
	fileNameBase = GenerateFileName(data->filename, "BaseLine");

	double y = data->oriData[0].y;

	//记录峰信息 为RecordInfo函数提供参数
	int start = 0, end = 0;

	QString strSZA = "";
	QString strBase = "";

	for (int i = m; i < num - m; ++i)
	{
		double temp = 0;
		for (int j = -m; j <= m; ++j)
		{
			temp += SZA_C(j) * (data->oriData[i + j].y - y);
		}

		temp = sqrt(fabs(temp));

		if (temp > f)
		{
			strSZA = strSZA + QString("%1 %2\n").arg(data->oriData[i].x, 0, 'f', 3).arg(data->oriData[i].y, 0, 'f', 3);

			if (end == 0)
			{
				start = i;
				++end;
			}
		}

#if 1
		//非谱峰区域直接输出，可作为直线本底谱
		else
		{
			strBase = strBase + QString("%1 %2\n").arg(data->oriData[i].x, 0, 'f', 3).arg(data->oriData[i].y, 0, 'f', 3);

			if (start != 0)
			{
				end = i - 1;
				if (Peaks == NULL)
				{
					Peaks = (PeakNode*)malloc(sizeof(PeakNode));

					Peaks->indStart = start;
					Peaks->indEnd = end;
					Peaks->indWidth = end - start;
					Peaks->next = NULL;

					Rear = Peaks;
				}
				else
				{
					PeakNode* temp = (PeakNode*)malloc(sizeof(PeakNode));
					temp->indStart = start;
					temp->indEnd = end;
					temp->indWidth = end - start;
					temp->next = NULL;

					Rear->next = temp;
					Rear = temp;
				}
				end = 0;
				start = 0;
			}
		}
#endif
	}

	WriteData(fileName, strSZA);
	WriteData(fileNameBase, strBase);
	return 0;
}

QString PeakFinding::GenerateFileName(QString filename, QString type) {
	int length = filename.size();
	QString file = filename.mid(0, length - 4);
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("_yyyy_MM_dd_hh") + QString::fromLocal8Bit("时") +
		current_date_time.toString("mm") + QString::fromLocal8Bit("分") +
		current_date_time.toString("ss") + QString::fromLocal8Bit("秒");
	QString newFilename = file + "_" + type + current_date + ".txt";
	return newFilename;
}

bool PeakFinding::WriteData(QString filename, QString str) {
	QFile file(filename);
	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QTextStream stream(&file);
		stream.seek(file.size());
		stream << str;
		file.close();
		return true;
	}
	return false;
}

bool PeakFinding::WriteData(QString filename, PeakNode* Peaks, int* TestData, int PeaksThreshold, DataManager* data) {
	QFile file(filename);

	PeakNode* p = Peaks;

	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QTextStream stream(&file);
		stream.seek(file.size());

		while (p != NULL) {
			if (abs(TestData[p->indPeak] - TestData[p->indStart]) >= PeaksThreshold) //设置阈值，过滤掉小峰RoundRatio*80/100
			{
				QString str = QString("%1 %2").arg(data->oriData[p->indStart].x, 0, 'f', 3).arg(data->oriData[p->indStart].y, 0, 'f', 3);
				stream << str << endl;
				str = QString("%1 %2").arg(data->oriData[p->indPeak].x, 0, 'f', 3).arg(data->oriData[p->indPeak].y, 0, 'f', 3);
				stream << str << endl;
				str = QString("%1 %2").arg(data->oriData[p->indEnd].x, 0, 'f', 3).arg(data->oriData[p->indEnd].y, 0, 'f', 3);
				stream << str << endl;
			}
			p = p->next;
		}

		p = NULL;

		file.close();

		return true;
	}
	return false;
}