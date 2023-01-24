#include "Include.h"

PlotWindow::PlotWindow(QWidget* parent):QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose, false);//父窗口关闭子窗口也关闭
    setWindowFlags(Qt::Tool | Qt::NoDropShadowWindowHint | 
        Qt::WindowStaysOnTopHint);
}

PlotWindow::~PlotWindow() {

}

void PlotWindow::paintEvent(QPaintEvent* event) {
    QPainterPath path;
    path.setFillRule(Qt::WindingFill);
    path.addRect(10, 10, this->width() - 20, this->height() - 20);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillPath(path, QBrush(Qt::white));

    QColor color(0, 0, 0, 50);
    for (int i = 0; i < 10; i++)
    {
        color.setAlpha(150 - sqrt(i) * 50);
        painter.setPen(color);
        painter.drawPath(path);
    }
}