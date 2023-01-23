#include "Include.h"

SideBar::SideBar(QWidget* parent) :QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
	hide();
	setAttribute(Qt::WA_QuitOnClose, false);
}

SideBar::~SideBar() {

}

void SideBar::paintEvent(QPaintEvent* event)
{
	//绘制背景色，如果不会绘制则会继承父窗口的透明背景
	QPainter p(this);
	p.fillRect(rect(), QColor(250, 250, 250, 250));
	
	////绘制圆角
	//QPainterPath path;
	//path.setFillRule(Qt::WindingFill);
	//QRectF rect(5, 5, this->width() - 10, this->height() - 10);
	//path.addRoundRect(rect, 2, 2);

	////绘制阴影
	//QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing, true);
	//painter.fillPath(path, QBrush(Qt::white));

	//QColor color(0, 0, 0, 50);
	//for (int i = 0; i < 5; i++) {
	//	QPainterPath path;
	//	path.setFillRule(Qt::WindingFill);
	//	path.addRect(5 - i, 5 - i, this->width() - (5 - i) * 2, this->height() - (5 - i) * 2);
	//	color.setAlpha(150 - sqrt(i + 5) * 50);
	//	painter.setPen(color);
	//	painter.drawPath(path);
	//}
}