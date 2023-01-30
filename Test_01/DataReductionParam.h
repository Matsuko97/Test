#pragma once

#include <QDialog>
#include "ui_DataReductionParam.h"

class DataReductionParam :public QDialog
{
	Q_OBJECT

public:
	DataReductionParam(QDialog* parent = nullptr);
	~DataReductionParam();

private:
	Ui::DataReductionParam ui;
};