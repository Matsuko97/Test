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

class PeakNode {
public:
    int indPeak;
    int indStart;
    int indEnd;
    int indWidth;
    PeakNode* next;

    PeakNode() {
        indPeak = 0;
        indStart = 0;
        indEnd = 0;
        indWidth = 0;
        next = nullptr;
    }
    PeakNode(const PeakNode& p) {
        indPeak = p.indPeak;
        indStart = p.indStart;
        indEnd = p.indEnd;
        indWidth = p.indWidth;
        next = nullptr;
    }
    ~PeakNode();

    PeakNode* CopyNode() {
        if (this == nullptr)
            return nullptr;

        PeakNode* cur = this;
        while (cur != nullptr) {
            PeakNode* temp = new PeakNode(*cur);
            temp->next = cur->next;
            cur->next = temp;
            cur = temp->next;
        }

        cur = this;
        PeakNode* res = cur->next;
        PeakNode* p = res;
        while (cur != nullptr && p != nullptr) {
            PeakNode* next = cur->next;
            cur->next = next;
            p->next = next == nullptr ? nullptr : next->next;
            cur = cur->next;
            p = p->next;
        }
        return res;
    }
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
    PeakNode* Peaks;

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
        Peaks = nullptr;
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

        Peaks = d.Peaks->CopyNode();
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