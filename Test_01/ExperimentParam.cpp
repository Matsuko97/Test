#include "Include.h"

Experiment::Experiment() {
    theory = BET;
    MolecularWeight = 0.0;
    MolecularDensity = 0.0;
    NumberOfData = 0;
    oriData = NULL;
    xyData = NULL;
    specificArea = 0.0;
}

Experiment::~Experiment() {
    delete oriData;
    delete xyData;
}

int Experiment::usingBET() {
    for (int i = 0; i < NumberOfData; ++i) {
        xyData[i].x = oriData[i].x;
        xyData[i].y = xyData[i].x / (oriData[i].y * (1 - xyData[i].x));
    }
    return 0;
}

int Experiment::usingLangmuir() {
    for (int i = 0; i < NumberOfData; ++i) {
        xyData[i].x = oriData[i].x / 760;//p/p0
        xyData[i].y = xyData[i].x / oriData[i].y;//p/(p0*v)
    }
    return 0;
}

void Experiment::LineFit() {
    double sumX2 = 0.0;
    double sumY = 0.0;
    double sumX = 0.0;
    double sumXY = 0.0;

    for (int i = 0; i < NumberOfData; ++i) {
        sumX2 += xyData[i].x * xyData[i].x;
        sumY += xyData[i].y;
        sumX += xyData[i].x;
        sumXY += xyData[i].x * xyData[i].y;
    }

    if (NumberOfData == 1) {
        lineFitInfo.slope = sumY / sumX;
        lineFitInfo.intercept = 0;
    }
    else {
        lineFitInfo.slope = (NumberOfData * sumXY - sumX * sumY) / (NumberOfData * sumX2 - sumX * sumX);
        lineFitInfo.intercept = (sumX2 * sumY - sumX * sumXY) / (NumberOfData * sumX2 - sumX * sumX);
    }
    return;
}

void Experiment::CalcSpecificArea() {
    double am;
    double Vm, vm;
    vm = MolecularWeight / (MolecularDensity * AVOGADRO);        //molecularMass�Ƿ�������N2:28����density���ܶ�(N2��0.808g/cm^3)��AVOGADRO�ǰ���٤���޳���Na
    am = pow(3, 0.5) / pow(2, 2 / 3.0) * pow(vm, 2 / 3.0);  //am��ÿ��������ռ�������λcm2    vm��ÿ��������ռ���
    am /= 10000;   //��λ���㣬m2
    switch (theory)
    {
    case Langmuir:
        Vm = 1 / lineFitInfo.slope;
        Vm /= 1000.0;
        specificArea = Vm * AVOGADRO * am;       //�ȱ������S=Vm * Na * am
        break;

    case BET:
        Vm = 1 / (lineFitInfo.slope + lineFitInfo.intercept);
        specificArea = Vm * AVOGADRO * am / 22400;       //�ȱ������S=Vm * Na * am
        break;

    case SingleBET:
        Vm = 1 / lineFitInfo.slope;
        specificArea = Vm * AVOGADRO * am / 22400;       //�ȱ������S=Vm * Na * am
        break;
    }
    return;
}

void Experiment::CalcProcess() {
    if (oriData != NULL) {
        switch (theory) {
        case Langmuir:
            usingLangmuir();
            break;

        case BET:
            usingBET();
            break;

        case SingleBET:
            usingBET();
            break;
        }

        LineFit();
        CalcSpecificArea();
    }
}