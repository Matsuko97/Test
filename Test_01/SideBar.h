#pragma once

#include <QtWidgets/QMainWindow>
#include <QPainter>
#include <QPainterPath>
#include "ui_SideBar.h"

class SideBar :public QWidget
{
	Q_OBJECT

public:
	SideBar(QWidget* parent = nullptr);
	~SideBar();


protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	Ui::SideBar ui;
};