#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Test.h"
#include "ExperimentParam.h"
#include "PeakFindingAlgorithm.h"
#include "PlotWindow.h"
#include "SideBar.h"

class Test : public QMainWindow
{
    Q_OBJECT

public:
    Test(QWidget *parent = nullptr);
    ~Test();

    bool GetData(QString filename);
    void RegExp(QString& str, double& data1, double& data2);
    QString GenerateFileName(QString filename);
    bool WriteData(QString name, int num, Data* data);
    bool WriteData(QString name, PeakNode* Peaks, int* TestData, int PeaksThreshold);
    
    //Peak-Finding Algorithm ******** Trend Accumulation
    void TrendAccumulation(Experiment exp);
    void FindPeaks(int* src, int ReadNums);
    int ScreenPeaks(int* src);
    void FreeLink(PeakNode* p);


private slots:
    void FileOpen();
    void SaveFile();
    void ShowPlotWindow();
    void ShowSideBar();

public:
    PlotWindow* plot;
    SideBar* sidebar;

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

private:
    Experiment exp;
    Ui::TestClass ui;
};
