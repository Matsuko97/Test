#include "Include.h"

Instrument::Instrument(QDialog* parent) :QDialog(parent)
{
	ui.setupUi(this);
	setFixedSize(761, 449);
	//setStyleSheet("border-image:url(bg.bmp)");

	process = new Process();
	openPixmap = new QPixmap("open.png");
	openPixmap->scaled(ui.value1->size(), Qt::KeepAspectRatio);
	closePixmap = new QPixmap("close.png");
	closePixmap->scaled(ui.value1->size(), Qt::KeepAspectRatio);

	ui.value1->setScaledContents(true);
	ui.value1->setPixmap(*openPixmap);
	values[0] = ui.value1;

	ui.value2->setScaledContents(true);
	ui.value2->setPixmap(*openPixmap);
	values[1] = ui.value2;

	ui.value3->setScaledContents(true);
	ui.value3->setPixmap(*openPixmap);
	values[2] = ui.value3;

	ui.value4->setScaledContents(true);
	ui.value4->setPixmap(*openPixmap);
	values[3] = ui.value4;

	ui.value5->setScaledContents(true);
	ui.value5->setPixmap(*closePixmap);
	values[4] = ui.value5;

	ui.value6->setScaledContents(true);
	ui.value6->setPixmap(*closePixmap);
	values[5] = ui.value6;

	ui.value7->setScaledContents(true);
	ui.value7->setPixmap(*openPixmap);
	values[6] = ui.value7;

	ui.value8->setScaledContents(true);
	ui.value8->setPixmap(*closePixmap);
	values[7] = ui.value8;

	currentTime = 0;
	totalTime = process->HeatingTime + process->RecoveryTime 
			+ process->AdsorptionTime + process->DesorptionTime;
	ui.progressBar->setRange(0, totalTime*1000);
	ui.progressBar->setValue(0);
	ui.progressBar->show();
	timer = new QTimer(this);
	timer->setInterval(1000);

	connect(process, &Process::ControlWordSig, this, &Instrument::OnControlWord);
	connect(ui.Vm, SIGNAL(toggled(bool)), this, SLOT(OnProcessThread()));
	connect(ui.FlowMeasurement, SIGNAL(toggled(bool)), this, SLOT(OnHeFlowMeasurement()));
	connect(ui.TotalFlowMeasurement, SIGNAL(toggled(bool)), this, SLOT(OnTotalFlowMeasurement()));
	connect(ui.Calibration, SIGNAL(toggled(bool)), this, SLOT(OnCalibration()));
	connect(ui.pushButton, &QPushButton::clicked, this, &Instrument::OnStart);
	connect(process, &Process::EndThreadSig, this, &Instrument::OnEnd);
	connect(this, &Instrument::processSig, process, &Process::OnProcessSig);
	connect(timer, &QTimer::timeout, this, &Instrument::onTimeout);
	connect(process, &Process::TimerStart, this, [=]() {
		timer->start(1000);
		});
	connect(process, &Process::Error, this, [=](QString strInfo) {
		QMessageBox::critical(NULL, "Error", strInfo);
		ui.pushButton->setEnabled(true);
		});
	connect(process, &Process::ShowValue, this, [=](double value) {
		if (value > DBL_EPSILON) {
			ui.textEdit_3->setText(QString::number(value, 'f', 3));
		}
		});
	connect(ui.btnCloseAll, &QPushButton::clicked, this, [=](bool click) {
		for (int i = 0; i < 8; ++i) {
			values[i]->clear();
			values[i]->setScaledContents(true);
			values[i]->setPixmap(*closePixmap);
		}
		});
}

Instrument::~Instrument() {
	delete process;
	process = nullptr;

	delete openPixmap;
	openPixmap = nullptr;

	delete closePixmap;
	closePixmap = nullptr;
}

void Instrument::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.drawPixmap(rect(), QPixmap("bg.bmp"), QRect());
}

void Instrument::closeEvent(QCloseEvent* e) {
	if (QMessageBox::question(this,
		tr("Quit"),
		tr("Are you sure to quit Instrument?"),
		QMessageBox::Yes, QMessageBox::No)
		== QMessageBox::Yes) {

		e->accept();//不会将事件传递给组件的父组件
		process->quit();
	}
}
void Instrument::OnProcessThread() {
	if (ui.Vm->isChecked()) {
		process->SendControlWord(ControlWord::QVPROCESS);
		ui.progressBar->setValue(0);
	}
}

void Instrument::OnHeFlowMeasurement() {
	if (ui.FlowMeasurement->isChecked()) {
		process->SendControlWord(ControlWord::HeFLOW);
		ui.progressBar->setValue(0);
	}
}

void Instrument::OnTotalFlowMeasurement() {
	if (ui.TotalFlowMeasurement->isChecked()) {
		process->SendControlWord(ControlWord::TOTALFLOW);
		ui.progressBar->setValue(0);
	}
}

void Instrument::OnCalibration() {
	if (ui.Calibration->isChecked()) {
		process->SendControlWord(ControlWord::CALIBRATION);
		ui.progressBar->setValue(0);
	}
}

void Instrument::OnControlWord(int control) {
	int c = control >> 8;
	int v = control;
	for (int i = 0; i < 8; ++i) {
		int a = (c >> i) & 1;
		int b = (v >> i) & 1;
		if (a != b) {
			if (b) {
				values[i]->clear();
				values[i]->setScaledContents(true);
				values[i]->setPixmap(*closePixmap);
			}
			else {
				values[i]->clear();
				values[i]->setScaledContents(true);
				values[i]->setPixmap(*openPixmap);
			}
		}
	}
}

void Instrument::OnStart() {
	process->start();
	if (ui.Vm->isChecked()) {
		ui.progressBar->setRange(0, totalTime * 1000);

		ui.FlowMeasurement->setEnabled(false);
		ui.TotalFlowMeasurement->setEnabled(false);
		ui.Calibration->setEnabled(false);

		emit processSig(ControlWord::QVPROCESS);
	}
	else if (ui.FlowMeasurement->isChecked()) {
		ui.progressBar->setRange(0, 100);

		ui.Vm->setEnabled(false);
		ui.TotalFlowMeasurement->setEnabled(false);
		ui.Calibration->setEnabled(false);

		emit processSig(ControlWord::HeFLOW);
	}
	else if (ui.TotalFlowMeasurement->isChecked()) {
		ui.progressBar->setRange(0, 100);

		ui.Vm->setEnabled(false);
		ui.FlowMeasurement->setEnabled(false);
		ui.Calibration->setEnabled(false);

		emit processSig(ControlWord::TOTALFLOW);
	}
	else if (ui.Calibration->isChecked()) {
		ui.Vm->setEnabled(false);
		ui.FlowMeasurement->setEnabled(false);
		ui.TotalFlowMeasurement->setEnabled(false);

		emit processSig(ControlWord::CALIBRATION);
	}
	ui.pushButton->setEnabled(false);
}

void Instrument::OnEnd(double n) {
	process->quit();
	ui.pushButton->setEnabled(true);
	ui.Vm->setEnabled(true);
	ui.FlowMeasurement->setEnabled(true);
	ui.TotalFlowMeasurement->setEnabled(true);
	ui.Calibration->setEnabled(true);
	ui.progressBar->setValue(100);

	if (n > DBL_EPSILON) {
		ui.textEdit_3->setText(QString::number(n, 'f', 3));
	}
}

void Instrument::onTimeout() {
	// 更新当前值
	currentTime+=1000;
	ui.progressBar->setValue(currentTime);
	ui.progressBar->show();
	// 判断是否达到总时间
	if (currentTime == 7000) {
		timer->stop();
		QMessageBox::information(this, "Progress", "Time is up!");
		ui.pushButton->setEnabled(true);
		ui.Vm->setEnabled(true);
		ui.FlowMeasurement->setEnabled(true);
		ui.TotalFlowMeasurement->setEnabled(true);
		ui.Calibration->setEnabled(true);
		currentTime = 0;
	}
}

Process::Process() {
	HeatingTime = 5.0;
	HeatingTemperature = 120.0;
	RecoveryTime = 0.0;
	AdsorptionTime = 0.0;
	DesorptionTime = 0.0;

	CarrierGasFlow = 0.0;
	TotalFlow = 0.0;

	Values = 0;
	controlWord = ControlWord::QVPROCESS;
	Values |= (int)controlWord;

	_pMutex = new QMutex;
}

Process::~Process() {
	delete  _pMutex;
	_pMutex = nullptr;
}

void Process::ProcessThread() {
	_pMutex->lock();
	if(1){
	//if (fabs(HeatingTime) > DBL_EPSILON && fabs(HeatingTemperature) > DBL_EPSILON
	//	&& fabs(RecoveryTime) > DBL_EPSILON && fabs(AdsorptionTime) > DBL_EPSILON
	//	&& fabs(DesorptionTime) > DBL_EPSILON) {

		controlWord = ControlWord::QVPROCESS;
		SendControlWord();
		//开启热导池开关

		emit TimerStart();
	}
	else {
		QString strInfo = QString::fromLocal8Bit("数据缺失");
		emit Error(strInfo);
	}

	_pMutex->unlock();
}

void Process::Calibration() {
	_pMutex->lock();

	//SendMessage发送阀门状态控制字
	controlWord = ControlWord::CALIBRATION;
	SendControlWord();

	_pMutex->unlock();
}

void Process::TotalFlowMeasurement() {
	_pMutex->lock();

	//改变阀门状态
	controlWord = ControlWord::TOTALFLOW;
	SendControlWord();

	//开始标定
	//doSomething(……)

	//测量流量
	int i = 0;
	while (i < 10) {  //连续测量保持稳定后输出
		double flow = GetMyInfo(flowrate, TotalFlow);
		msleep(50);//使用sleep模拟测量的过程
		emit ShowValue(flow);

		if (abs(flow - TotalFlow) <= 5) {
			++i;
		}
		else {
			i = 0;
		}
		TotalFlow = flow;
	}

	emit EndThreadSig(TotalFlow);
	_pMutex->unlock();
}

void Process::HeFlowMeasurement() {
	_pMutex->lock();

	//改变阀门状态
	controlWord = ControlWord::HeFLOW;
	SendControlWord();

	//测量流量
	int i = 0;
	while (i < 10) {  //连续测量保持稳定后输出
		double flow = GetMyInfo(flowrate, CarrierGasFlow);
		msleep(50);//使用sleep模拟测量的过程
		emit ShowValue(flow);

		if (abs(flow - CarrierGasFlow) <= 5) {
			++i;
		}
		else {
			i = 0;
		}
		CarrierGasFlow = flow;
	}

	emit EndThreadSig(CarrierGasFlow);
	_pMutex->unlock();
}

void Process::SendControlWord() {
	int sendValue = Values | ((int)controlWord << 8);
	//发送sendValue

	emit ControlWordSig(sendValue);

	Values &= 0;
	Values |= (int)controlWord;
	return;
}

void Process::SendControlWord(ControlWord c) {
	int sendValue = Values | ((int)c << 8);
	emit ControlWordSig(sendValue);
	Values &= 0;
	Values |= (int)c;
	return;
}

double Process::GetMyInfo(Type type, double ori) {
	//仅用来模拟参数变化，底层通讯模块完善后此部分应当为接受传感器数值
	double ret = ori;

	//随机数初始化，仅与GetMyInfo（）搭配用于模拟参数变化
	srand(QTime::currentTime().msec());

	switch (type) {
	case tempurature:
		ret += (rand() % 20);
		break;

	case flowrate:
		if (ret < 700) {
			ret += (rand() % 50) + 5;
		}
		else {}
		break;
	}
	return ret;
}

void Process::OnProcessSig(ControlWord c) {
	controlWord = c;
	return;
}

void Process::run() {
	switch (controlWord) {
	case ControlWord::QVPROCESS:
		ProcessThread();
		break;

	case ControlWord::HeFLOW:
		HeFlowMeasurement();
		break;

	case ControlWord::TOTALFLOW:
		TotalFlowMeasurement();
		break;

	case ControlWord::CALIBRATION:
		Calibration();
		break;
	}

	exec();
}