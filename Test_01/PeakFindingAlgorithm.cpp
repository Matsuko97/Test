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

	if (Peaks == NULL) //Ƭ�����޷�
	{
	}

	PeaksThreshold = ScreenPeaks(TestData);
	
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

		if (accumulation > 0)              //>�жϲ���
		{
			if (count == 0)            //�ж��Ƿ����µĲ���
			{
				if (Flag)              //�ж�ͷ�ڵ��Ƿ��Ѿ�����
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
				p->indStart = i;       //���忪ʼ���¼
				p->indPeak = i + 1;    //Ŀǰ�ķ�ֵ���¼

			}
			p->indEnd = i + 1;        //Ŀǰ�ķ�β���¼
			if (src[i + 1] > peakThreshold)    //Ѱ�ҷ�ֵ��,��ʵ�ʵ�ֵ����
			{
				peakThreshold = src[i + 1];
				p->indPeak = i + 1;

			}
			count++;               //��¼��ɲ���ĵ���
		}

		if (accumulation < 0)         //�жϲ���
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

		if (accumulation == 0 && count != 0)     //���岨��β���ж�
		{
			p->indEnd = i + 1;
			count = 0;
			peakThreshold = 0;
		}

		//���ڲ���򲨹��ѵ���β�������ӺͲ�Ϊ������
		if (count > 500)                    //��������߲������ݳ���һ������������500
		{
			if (abs(src[p->indEnd] - peakThreshold) <= RoundRatio / 10 && abs(peakThreshold - src[p->indStart]) >= RoundRatio * 80 / 100)//��Ŀ������������Ѿ��ﵽ500����ʵ�ʷ廹δ�ߵ���Ҫȷ��β���ĵط��Ҵ˷�Ϊ��Ч��ʱ��������β���ж�
			{
				count = count - 100;
			}
			else
			{
				maxDiff = 0;
				diff = 0;
				for (j = i; j < i + 100; j++)    //����501���100��ÿ������501��Ĳ�ֵ��ȡ���ֵ
				{
					diff = src[j] - src[i];
					if (abs(diff) > maxDiff)
					{
						maxDiff = abs(diff);
					}
				}

				if (maxDiff <= RoundRatio / 10)   //�����ֵС���趨����ֵ����Ϊ�Ѿ��ﵽβ��
				{
					p->indEnd = i + 1;
					count = 0;
					peakThreshold = 0;
					accumulation = 0;  //���ڲ���򲨹��ѵ���β�������ӺͲ�Ϊ0������轫�Ӻ͹���
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

int PeakFinding::ScreenPeaks(int* src)   // ������⵽�ķ壬ȡ��ֵ�������ķ����Ϊ��ֵ
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

	result = - exp(-4 * (j * j / H / H) * log(2.0));
	//result = - (H * H) / (H * H + j * j);
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

	double y = data->oriData[0].y;

	//��¼����Ϣ
	int start = 0, end = 0;

	QString strSZA = "";
	QString strBase = "";

	for (int i = m; i < num - m; ++i)
	{
		double temp = 0.0;
		double temp2 = 0.0;
		for (int j = -m; j <= m; ++j){
			temp += SZA_C(j) * (data->oriData[i + j].y - y);

			temp2 += pow(SZA_C(j), 2) * (data->oriData[i + j].y - y);
		}
		//temp = sqrt(fabs(temp));
		temp = temp / sqrt(fabs(temp2));

		if (temp > f){
			strSZA = strSZA + QString("%1 %2\n").arg(data->oriData[i].x, 0, 'f', 3).arg(data->oriData[i].y, 0, 'f', 3);

			if (end == 0){
				start = i;
				++end;
			}
		}

#if 1
		//���׷�����ֱ�����������Ϊֱ�߱�����
		else{
			strBase = strBase + QString("%1 %2\n").arg(data->oriData[i].x, 0, 'f', 3).arg(data->oriData[i].y, 0, 'f', 3);

			if (start != 0)
			{
				end = i - 1;
				if (Peaks == nullptr){
					Peaks = (PeakNode*)malloc(sizeof(PeakNode));

					Peaks->indStart = start;
					Peaks->indEnd = end;
					Peaks->indWidth = end - start;
					Peaks->next = nullptr;

					Rear = Peaks;
				}
				else{
					PeakNode* temp = (PeakNode*)malloc(sizeof(PeakNode));
					temp->indStart = start;
					temp->indEnd = end;
					temp->indWidth = end - start;
					temp->next = nullptr;

					Rear->next = temp;
					Rear = temp;
				}
				end = 0;
				start = 0;
			}
		}
#endif
	}

	WriteData(data->filePeak, strSZA);
	WriteData(data->fileBase, strBase);
	return 0;
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

		while (p != nullptr){
			if (abs(TestData[p->indPeak] - TestData[p->indStart]) >= PeaksThreshold) //������ֵ�����˵�С��RoundRatio*80/100
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

void PeakFinding::BasicSNIP(DataManager* data, int num, int m, int i, bool type, QString & str)
{

	double y = 0.0;
	double yPlusm = 0.0;
	double yMinusm = 0.0;

	//�����ݽ���LLS�任
	y = log(log(sqrt(fabs(data->oriData[i].y) + 1) + 1) + 1);

	double yPlusOriginal = 0.0;
	double yMinusOriginal = 0.0;

	for (int j = 1; j <= m; ++j)
	{
		//��ֹ����ֵÿ�ǰ��ʹ��û���������ڼ���
		if (i - j < 0){
			yMinusOriginal = data->oriData[i].y;
		}
		else{
			yMinusOriginal = data->oriData[i - j].y;
		}

		//��ֹ����ֵÿ���û���������ڼ���
		if (i + j >= num){
			yPlusOriginal = data->oriData[num - 1].y;
		}
		else{
			yPlusOriginal = data->oriData[i + j].y;
		}

		//�����ݽ���LLS�任
		yPlusm = log(log(sqrt(fabs(yPlusOriginal) + 1) + 1) + 1);
		yMinusm = log(log(sqrt(fabs(yMinusOriginal) + 1) + 1) + 1);

		//SNIP�㷨�߼�
		if (type)//���ֲ��壬����SNIP�㷨�жϼ���
		{
			y = y < ((yPlusm + yMinusm) / 2) ? y : (yPlusm + yMinusm) / 2;
		}
		else//���ֲ��ȣ���Ҫ�����෴�߼��ж�
		{
			y = y > ((yPlusm + yMinusm) / 2) ? y : (yPlusm + yMinusm) / 2;
		}
	}

	//�Դ����ı��������ݽ��з�LLS�任
	y = pow((exp(exp(y) - 1) - 1), 2) - 1;

	///////
	str = str + QString("%1 %2\n").arg(data->oriData[i].x, 0, 'f', 3).arg(y, 0, 'f', 3);
	///////��������ڸĽ���SNIP�㷨���������������Ϣ

	return;
}

void PeakFinding::ImprovedSNIP(DataManager* data, PeakNode* Head){
	int num = data->NumberOfData;
	QString str = "";
	PeakNode* p = Head;
	int start = 0, end = 0, width = 0;
	bool flag = true;//�жϵ�ǰ�Ƿ���Ҫ��ȡ�����Ϣ

	for (int i = 0; i < num; ++i)
	{
		if (flag){
			ObtainInfo(p, start, end, width, num);
			flag = false;
		}
		else{
		}

		if (i >= start && i < end){
			BasicSNIP(data, num, width, i, false, str);
		}
		else if (i == end){
			flag = true;
			//��Ҫ���»�ȡ��һ�������Ϣ����SNIP�㷨���ڿ�Ƚ��е���
			
			str = str + QString("%1 %2\n").arg(data->oriData[i].x, 0, 'f', 3).arg(data->oriData[i].y, 0, 'f', 3);
		}
		else{
			BasicSNIP(data, num, width, i, false, str);
		}
	}

	//----------ע�͵������������䣬ֻд�����forѭ���������Թ̶���ȴ�����������---------//
	//for( int i = 0 ; i<num ;++i )
	//{
	//	BasicSNIP( dataNew, num, /*width*/ 31, i, false);
	//}//�̶���ȴ�����������

	WriteData(data->fileSNIP, str);
	return;
}

void PeakFinding::ObtainInfo(PeakNode* p, int& start, int& end, int& width, int num){
	if (p != nullptr){
		start = p->indStart;
		end = p->indEnd;
		width = p->indWidth;
		p = p->next;
	}
	else{
		start = 0;
		end = num;
		width = 0;
	}
	return;
}