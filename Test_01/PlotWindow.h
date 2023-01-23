#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PlotWindow.h"

class PlotWindow:public QMainWindow
{
	Q_OBJECT

public:
	PlotWindow(QWidget* parent = nullptr);
	~PlotWindow();

private:
	Ui::PlotWindow ui;
};