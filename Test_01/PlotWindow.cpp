#include "Include.h"

PlotWindow::PlotWindow(QWidget* parent):QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose, false);//�����ڹر��Ӵ���Ҳ�ر�
    setAttribute(Qt::WA_DeleteOnClose);
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

    return;
}