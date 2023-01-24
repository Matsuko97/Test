#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Test.h"
#include "Calculation.h"
#include "PeakFindingAlgorithm.h"
#include "PlotWindow.h"
#include "SideBar.h"

class Test : public QMainWindow
{
    Q_OBJECT

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
    void ShowSideBar();
    void OnPeakFinding();
    void OnCalculation();

public:
    PlotWindow* plot;
    SideBar* sidebar;
    DataManager* dataManager;

protected:
    virtual void resizeEvent(QResizeEvent* event) override;

private:
    Ui::TestClass ui;
};
