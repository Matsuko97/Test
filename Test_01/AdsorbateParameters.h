#pragma once

#include <QDialog>
#include "ui_AdsorbateParameters.h"

class AdsorbateParameters :public QDialog
{
	Q_OBJECT

public:
	AdsorbateParameters(QDialog* parent = nullptr);
	~AdsorbateParameters();

private:
	Ui::AdsorbateParameters ui;
};