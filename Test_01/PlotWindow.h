#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PlotWindow.h"
#include "Calculation.h"
#include "CustomDialog.h"

class PlotWindow:public QWidget
{
	Q_OBJECT

public:
	PlotWindow(QWidget* parent = nullptr);
	//~PlotWindow();
	
protected:
	void paintEvent(QPaintEvent* event) override;

public:
	void drawPlot(Data* data, int n, QString s);

public slots:
	void selectionChanged();
	void ShowColor(const QColor& color);
	void Set(const QColor& color);

public:
	CustomDialog* m_pColor;

private:
	Ui::PlotWindow ui;
};