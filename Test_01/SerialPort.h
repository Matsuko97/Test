#pragma once

#include <QDialog>
#include "ui_SerialPort.h"

class SerialPort :public QDialog
{
	Q_OBJECT

public:
	SerialPort(QDialog* parent = nullptr);
	~SerialPort();

private:
	Ui::SerialPort ui;
};