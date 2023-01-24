#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PlotWindow.h"

class PlotWindow:public QWidget
{
	Q_OBJECT

public:
	PlotWindow(QWidget* parent = nullptr);
	~PlotWindow();
	
protected:
	void paintEvent(QPaintEvent* event) override;
	 
private:
	Ui::PlotWindow ui;
};