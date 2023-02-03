#pragma once

#include <QDialog>
#include "ui_Instrument.h"

class Instrument :public QDialog
{
	Q_OBJECT

public:
	Instrument(QDialog* parent = nullptr);
	~Instrument();

protected:
	virtual void paintEvent(QPaintEvent* e) override;

private:
	Ui::Instrument ui;
};