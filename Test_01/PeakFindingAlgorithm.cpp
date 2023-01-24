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

	if (Peaks == NULL) //Ƭ�����޷�
	{
		/*MessageBox(hwnd, TEXT("������ϣ��޷壡"), TEXT("Over"), MB_OK);
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