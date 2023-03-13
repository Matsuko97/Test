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
#include <QMenu>
#include <QMdiArea>

class Test : public QMainWindow
{
    Q_OBJECT

signals:
    void plotWindowResize();
    void dataReady(Data* data, int n, QString s);

public:
    Test(QWidget *parent = nullptr);
    ~Test();

    bool ReadData(QString filename);
    void RegExp(QString& str, double& data1, double& data2);
    QString GenerateFileName(QString filename, QString type);
    bool WriteData(QString filename, int num, Data* data); 
    Data* ReadDataForPlot(QString filename, int& count);
    PlotWindow* showPlot();

private slots:
    void FileOpen();
    void SaveFile(QCPGraph* graph);
    void ShowPlotWindow();
    void showContextMenu(QPoint pos);
    void OnPlotWindowResize();
    void ShowSideBar();
    void OnFiltering(int i);
    void OnPeakFinding();
    void OnBaseLine();
    void OnCalculation();
    void OnCloseAll();
    void DrawPlot(Data* data, int n, QString s);
    void ShowInNewPlot();

public:
    DataManager* dataManager;
    DataManager* dataSmooth;
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
