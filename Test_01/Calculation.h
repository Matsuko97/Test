#pragma once

#define AVOGADRO  6.02 * pow(10.0, 23)

struct Parameter {
    //string name;
    double value;
};

struct Data {
    double x;
    double y;
};

enum Theory
{
    Langmuir = 1,
    BET,
    SingleBET
};

struct LineFitInfo {
    double slope;
    double intercept;
};


class DataManager {
public:
    Data* oriData;
    QString filename;
    int NumberOfData;

public:
    DataManager() {
        oriData = nullptr;
        NumberOfData = 0;
    }

    ~DataManager() {
        free(oriData);
    }

    Data* GetOriData() {
        return oriData;
    }
};

class Calculation {
public:
    //Parameter sorbentParam[5];
    Theory theory;
    double MolecularWeight;
    double MolecularDensity;
    Data* oriData;
    Data* xyData;
    int NumberOfData;
    LineFitInfo lineFitInfo;
    double specificArea;

public:
    Calculation();
    ~Calculation();

    int usingBET();
    int usingLangmuir();
    void LineFit();
    void CalcSpecificArea();
    void CalcProcess(DataManager* dataManager);
};