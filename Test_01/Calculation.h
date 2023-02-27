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
    int NumberOfData;
    int E;
    double Factor;
    QString filename;
    QString fileFilter;
    QString filePeak;
    QString fileBase;
    QString fileSNIP;

public:
    DataManager() {
        oriData = nullptr;
        NumberOfData = 0;
        E = 0;
        Factor = 1.0;
        filename = "";
        fileFilter = "";
        filePeak = "";
        fileBase = "";
        fileSNIP = "";
    }

    DataManager(const DataManager& d) {
        NumberOfData = d.NumberOfData;
        oriData = (Data* )calloc(NumberOfData, sizeof(Data));
        memcpy(oriData, d.oriData, NumberOfData*sizeof(Data));
        
        E = d.E;
        Factor = d.Factor;
        filename = d.filename;
        fileFilter = d.fileFilter;
        filePeak = d.filePeak;
        fileBase = d.fileBase;
        fileSNIP = d.fileSNIP;
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