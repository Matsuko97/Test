#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Test.h"
#include "Calculation.h"
#include "PeakFindingAlgorithm.h"
#include "PlotWindow.h"
#include "SideBar.h"
#include "DataReductionParam.h"
#include "Instrument.h"
#include "SerialPort.h"
#include "AdsorbateParameters.h"

class Test : public QMainWindow
{
    Q_OBJECT

signals:
    void plotWindowResize();
    void dataReady(Data* data, int n);

public:
    Test(QWidget *parent = nullptr);
    ~Test();

    bool ReadData(QString filename);
    void RegExp(QString& str, double& data1, double& data2);
    QString GenerateFileName(QString filename);
    bool WriteData(QString name, int num, Data* data);
    bool WriteData(QString name, PeakNode* Peaks, int* TestData, int PeaksThreshold);

private slots:
    void FileOpen();
    void SaveFile();
    void ShowPlotWindow();
    void OnPlotWindowResize();
    void ShowSideBar();
    void OnPeakFinding();
    void OnCalculation();
    void OnCloseAll();
    void DrawPlot(Data* data, int n);

public:
    DataManager* dataManager;
    Instrument* instrument;
    DataReductionParam* dataParams;
    AdsorbateParameters* adsorbate;
    SerialPort* serialPort;
    SideBar* sidebar;

    int subWindowCount;

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

private:
    Ui::TestClass ui;
};
