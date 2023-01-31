#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PlotWindow.h"
#include "Calculation.h"

class PlotWindow:public QWidget
{
	Q_OBJECT

public:
	PlotWindow(QWidget* parent = nullptr);
	~PlotWindow();
	
protected:
	void paintEvent(QPaintEvent* event) override;

public:
	void drawPlot(Data* data, int n);
	 
private:
	Ui::PlotWindow ui;
};