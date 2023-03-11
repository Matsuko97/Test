#pragma once

#include <QDialog>
#include <QThread>
#include <QMutex>
#include <QTimer>
#include "ui_Instrument.h"

enum Type {
    tempurature,
    flowrate
};

enum class ControlWord {
    HeFLOW = 0x001A,
    TOTALFLOW = 0x001B,
    QVPROCESS = 0X004F,
    CALIBRATION = 0x00E3
};

class Process : public QThread{
    Q_OBJECT

signals:
    void ControlWordSig(int control);
    void EndThreadSig(double n);
    void TimerStart();
    void Error(QString strInfo);
    void ShowValue(double value);

public:
    QString adsorbent; //吸附剂
    QString adsorbate; //吸附质
    QString carrierGas;//载气
    double HeatingTime;
    double HeatingTemperature;
    double RecoveryTime;
    double AdsorptionTime;
    double DesorptionTime;

    double CarrierGasFlow;
    double TotalFlow;

    int Values;
    /*
    int Values  0~7位表示阀门状态
                8~15位表示开or关几号阀门 即阀门控制
    0       SorbateValue
    1       CarrierGasValue
    2       SorbateCalibrationValue
    3       MixedGasMeasurementValue
    4       FlowMeasurementValue
    5       SampleTubeExitValue
    6       VentingValue
    7       MixedGasCalibrationValue
    */

    QMutex* _pMutex;
    //QTimer* timer;

private:
    ControlWord controlWord;

public:
    void ProcessThread();
    void Calibration();
    void HeFlowMeasurement();
    void TotalFlowMeasurement();
    void OnProcessSig(ControlWord c);

public:
    Process();
    ~Process();

    void SendControlWord();
    void SendControlWord(ControlWord c);
    double GetMyInfo(Type type, double ori);

    void run();
};

class Instrument :public QDialog
{
	Q_OBJECT

signals:
    void processSig(ControlWord c);

public:
	Instrument(QDialog* parent = nullptr);
	~Instrument();

protected:
	virtual void paintEvent(QPaintEvent* e) override;
    virtual void closeEvent(QCloseEvent* e) override;

public slots:
    void OnControlWord(int control);
    void OnProcessThread();
    void OnHeFlowMeasurement();
    void OnTotalFlowMeasurement();
    void OnCalibration();
    void OnStart();
    void OnEnd(double n);
    void onTimeout();

private:
    Process* process;
    QLabel* values[8];
    QPixmap* openPixmap;
    QPixmap* closePixmap;

    int currentTime;
    int totalTime;
    QTimer* timer;

private:
	Ui::Instrument ui;
};