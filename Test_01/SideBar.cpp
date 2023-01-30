#include "Include.h"

SideBar::SideBar(QWidget* parent) :QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	hide();
	setAttribute(Qt::WA_QuitOnClose, false);
	ui.splitter->setStretchFactor(0, 3);
	ui.splitter->setStretchFactor(1, 1);
	ui.splitter->setStretchFactor(2, 6);

	ui.plainTextEdit->clear();

	ui.plainTextEdit->insertPlainText("Class            Physisorption\n");
	ui.plainTextEdit->insertPlainText("Default method   BET\n");
	ui.plainTextEdit->insertPlainText("Heat Temp        0.00\n");
	ui.plainTextEdit->insertPlainText("Heat Time        0\n");
	ui.plainTextEdit->insertPlainText("Recovery         0\n");
	ui.plainTextEdit->insertPlainText("Ads. Temp        0.00\n");
	ui.plainTextEdit->insertPlainText("Ads. Time        0\n");
	ui.plainTextEdit->insertPlainText("Des. Time        0\n");
}

SideBar::~SideBar() {

}

void SideBar::paintEvent(QPaintEvent* event)
{
	//绘制背景色，如果不会绘制则会继承父窗口的透明背景
	QPainter p(this);
	p.fillRect(rect(), QColor(250, 250, 250, 250));
}