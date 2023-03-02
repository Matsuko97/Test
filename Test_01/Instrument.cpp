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

	connect(process, &Process::ControlWordSig, this, &Instrument::OnControlWord);
	connect(ui.Vm, SIGNAL(toggled(bool)), this, SLOT(OnProcessThread()));
	connect(ui.FlowMeasurement, SIGNAL(toggled(bool)), this, SLOT(OnHeFlowMeasurement()));
	connect(ui.TotalFlowMeasurement, SIGNAL(toggled(bool)), this, SLOT(OnTotalFlowMeasurement()));
	connect(ui.Calibration, SIGNAL(toggled(bool)), this, SLOT(OnCalibration()));
	connect(ui.pushButton, &QPushButton::clicked, this, &Instrument::OnStart);
	connect(process, &Process::EndThreadSig, this, &Instrument::OnEnd);
	connect(this, &Instrument::processSig, process, &Process::OnProcessSig);
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

	//process->start();
}

void Instrument::closeEvent(QCloseEvent* e) {
	if (QMessageBox::question(this,
		tr("Quit"),
		tr("Are you sure to quit Instrument?"),
		QMessageBox::Yes, QMessageBox::No)
		== QMessageBox::Yes) {

		e->accept();//不会将事件传递给组件的父组件
		process->quit();
		//process = nullptr;
	}
}
void Instrument::OnProcessThread() {
	if (ui.Vm->isChecked()) {
		process->SendControlWord(ControlWord::QVPROCESS);
	}
}

void Instrument::OnHeFlowMeasurement() {
	if (ui.FlowMeasurement->isChecked()) {
		process->SendControlWord(ControlWord::HeFLOW);
	}
}

void Instrument::OnTotalFlowMeasurement() {
	if (ui.TotalFlowMeasurement->isChecked()) {
		process->SendControlWord(ControlWord::TOTALFLOW);
	}
}

void Instrument::OnCalibration() {
	if (ui.Calibration->isChecked()) {
		process->SendControlWord(ControlWord::CALIBRATION);
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
		emit processSig(ControlWord::QVPROCESS);
	}
	else if (ui.FlowMeasurement->isChecked()) {
		emit processSig(ControlWord::HeFLOW);
	}
	else if (ui.TotalFlowMeasurement->isChecked()) {
		emit processSig(ControlWord::TOTALFLOW);
	}
	else if (ui.Calibration->isChecked()) {
		emit processSig(ControlWord::CALIBRATION);
	}
	ui.pushButton->setEnabled(false);
}

void Instrument::OnEnd(double n) {
	process->quit();
	ui.pushButton->setEnabled(true);
	if (n > DBL_EPSILON) {
		ui.textEdit_3->setText(QString::number(n, 'f', 3));
	}
}

Process::Process() {
	HeatingTime = 0.0;
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
	if (fabs(HeatingTime) > DBL_EPSILON && fabs(HeatingTemperature) > DBL_EPSILON
		&& fabs(RecoveryTime) > DBL_EPSILON && fabs(AdsorptionTime) > DBL_EPSILON
		&& fabs(DesorptionTime) > DBL_EPSILON) {

		controlWord = ControlWord::QVPROCESS;
		SendControlWord();
		//开启热导池开关

		/*//开启定时器

		SetTimer(hwnd, ……, 1000 * TIMEUNIT * HeatingTime);
		//需重写OnTimer OnTimer内部     1、测温     2、发送信号
		WaitForOneObject();
		SetTimer(hwnd, ……, 1000 * TIMEUNIT * RecoveryTime);
		WaitForOneObject();
		SetTimer(hwnd, ……, 1000 * TIMEUNIT * AdsorptionTime);
		WaitForOneObject();
		SetTimer(hwnd, ……, 1000 * TIMEUNIT * DesorptionTime);
		WaitForOneObject();

		*/

	}
	else {
		QString dlgTitle = "Error";
		QString strInfo = QString::fromLocal8Bit("数据缺失");
		//QMessageBox::critical(NULL, dlgTitle, strInfo);
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
		if (ret < 240) {
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
	QTimer* timer = new QTimer();
	switch (controlWord) {
	case ControlWord::QVPROCESS:
		timer->setInterval(10000);
		timer->start();
		ProcessThread();
		connect(timer, &QTimer::timeout, this, [=]() {
			emit EndThreadSig(0);
			});
		break;

	case ControlWord::HeFLOW:
		HeFlowMeasurement();
		break;

	case ControlWord::TOTALFLOW:
		TotalFlowMeasurement();
		break;

	case ControlWord::CALIBRATION:
		timer->setInterval(5000);
		timer->start();
		Calibration();
		connect(timer, &QTimer::timeout, this, [=]() {
			emit EndThreadSig(0);
			});
		break;
	}

	exec();
}