#pragma once

#include <QDialog>
#include "ui_Instrument.h"

class Instrument :public QDialog
{
	Q_OBJECT

public:
	Instrument(QDialog* parent = nullptr);
	~Instrument();

private:
	Ui::Instrument ui;
};