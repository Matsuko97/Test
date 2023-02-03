#include "Include.h"

Instrument::Instrument(QDialog* parent) :QDialog(parent)
{
	ui.setupUi(this);
	setFixedSize(761, 449);
	//setStyleSheet("border-image:url(bg.bmp)");
}

Instrument::~Instrument() {

}

void Instrument::paintEvent(QPaintEvent* e)
{
	QPainter painter(this);
	painter.drawPixmap(rect(), QPixmap("bg.bmp"), QRect());

	QPixmap* openPixmap = new QPixmap("open.png");
	openPixmap->scaled(ui.value1->size(), Qt::KeepAspectRatio);
	QPixmap* closePixmap = new QPixmap("close.png");
	closePixmap->scaled(ui.value1->size(), Qt::KeepAspectRatio);
	ui.value1->setScaledContents(true);
	ui.value1->setPixmap(*openPixmap);

	ui.value2->setScaledContents(true);
	ui.value2->setPixmap(*openPixmap);

	ui.value3->setScaledContents(true);
	ui.value3->setPixmap(*openPixmap);

	ui.value4->setScaledContents(true);
	ui.value4->setPixmap(*openPixmap);

	ui.value5->setScaledContents(true);
	ui.value5->setPixmap(*closePixmap);

	ui.value6->setScaledContents(true);
	ui.value6->setPixmap(*closePixmap);

	ui.value7->setScaledContents(true);
	ui.value7->setPixmap(*openPixmap);

	ui.value8->setScaledContents(true);
	ui.value8->setPixmap(*closePixmap);

}