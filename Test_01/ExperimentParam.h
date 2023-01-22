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

class Experiment {
public:
    //Parameter sorbentParam[5];
    QString filename;
    Theory theory;
    double MolecularWeight;
    double MolecularDensity;
    int NumberOfData;
    Data* oriData;
    Data* xyData;
    LineFitInfo lineFitInfo;
    double specificArea;

public:
    Experiment();
    ~Experiment();

    int usingBET();
    int usingLangmuir();
    void LineFit();
    void CalcSpecificArea();
    void CalcProcess();
};