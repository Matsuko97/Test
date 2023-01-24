#include "Include.h"

void PeakFinding::TrendAccumulation(DataManager* data) {
	Data* oriData = data->GetOriData();
	int num = data->NumberOfData;
	int j, PeaksThreshold = 0;
	int* TestData = (int*)calloc(data->NumberOfData, sizeof(int));
	PeakNode* p;

	for (j = 0; j < num; j++)
	{
		//TestData_y[j] = TestData[j].y ;
		TestData[j] = oriData[j].y * RoundRatio;
	}

	FindPeaks(TestData, num);

	if (Peaks == NULL) //片段中无峰
	{
		/*MessageBox(hwnd, TEXT("处理完毕，无峰！"), TEXT("Over"), MB_OK);
		return TRUE;*/
	}

	PeaksThreshold = ScreenPeaks(TestData);

	//WriteData(data->filename, Peaks, TestData, PeaksThreshold);
	//FreeLink(Peaks);
	//Peaks = NULL;
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