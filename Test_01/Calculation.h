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
    int E;
    double Factor;

public:
    DataManager() {
        oriData = nullptr;
        NumberOfData = 0;
    }

    ~DataManager() {
        free(oriData);
        oriData = nullptr;
    }

    Data* GetOriData() {
        return oriData;
    }

    void CalcExponent(double num) {
        int n = 0;
        if (num < 1)
        {
            while (num < 1)
            {
                num *= 10;
                n--;
            }
        }
        else
        {
            while (num > 10)
            {
                num /= 10;
                n++;
            }
        }
        
        E = n - 1;
        Factor = E ? pow(10, E) : 1;
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