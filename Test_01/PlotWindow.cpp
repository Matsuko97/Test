#include "Include.h"

PlotWindow::PlotWindow(QWidget* parent):QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_QuitOnClose, false);//父窗口关闭子窗口也关闭
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

void PlotWindow::drawPlot(Data* data, int n) {
    QVector<double> x(n), y(n);
    double xMin = data[0].x, xMax = data[0].x, yMin = data[0].y, yMax = data[0].y;
    for (int i = 0; i < n; ++i) {
        x[i] = data[i].x;
        y[i] = data[i].y;

        xMin = x[i] < xMin ? x[i] : xMin;
        xMax = x[i] > xMax ? x[i] : xMax;
        yMin = y[i] < yMin ? y[i] : yMin;
        yMax = y[i] > yMax ? y[i] : yMax;
    }

    double xL = xMin - (xMax - xMin) / 10;
    double xR = xMax + (xMax - xMin) / 10;
    double yB = yMin - (yMax - yMin) / 10;
    double yT = yMax + (yMax - yMin) / 10;

    ui.customPlot->addGraph();
    ui.customPlot->graph(0)->setData(x, y);
    ui.customPlot->graph(0)->setName("origin data");

    ui.customPlot->xAxis->setLabel("x");
    ui.customPlot->yAxis->setLabel("y");

    // 设置坐标轴的范围，以看到所有数据
    ui.customPlot->xAxis->setRange(xL, xR);
    ui.customPlot->yAxis->setRange(yB, yT);
    ui.customPlot->legend->setVisible(true); // 显示图例
    // 重画图像
    ui.customPlot->replot();
}