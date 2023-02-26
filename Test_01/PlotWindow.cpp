#include "Include.h"

QColor Pen[10] = { QColor(0,0,0), Qt::darkRed,
    Qt::darkGreen,
    Qt::black };

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

void PlotWindow::drawPlot(Data* data, int n, QString s) {
    int cnt = ui.customPlot->graphCount();

    QVector<double> x(n), y(n);
    //double xMin = data[0].x, xMax = data[0].x, yMin = data[0].y, yMax = data[0].y;
    for (int i = 0; i < n; ++i) {
        x[i] = data[i].x;
        y[i] = data[i].y;

        //xMin = x[i] < xMin ? x[i] : xMin;
        //xMax = x[i] > xMax ? x[i] : xMax;
        //yMin = y[i] < yMin ? y[i] : yMin;
        //yMax = y[i] > yMax ? y[i] : yMax;
    }

    //double xL = xMin - (xMax - xMin) / 10;
    //double xR = xMax + (xMax - xMin) / 10;
    //double yB = yMin - (yMax - yMin) / 10;
    //double yT = yMax + (yMax - yMin) / 10;

    // 设置坐标轴的范围，以看到所有数据
    //ui.customPlot->xAxis->setRange(xMin * 0.9, xMax * 1.1);
    //ui.customPlot->yAxis->setRange(yMin * 0.9, yMax * 1.1);

    ui.customPlot->xAxis2->setVisible(true);
    ui.customPlot->xAxis2->setTickLabels(false);
    ui.customPlot->yAxis2->setVisible(true);
    ui.customPlot->yAxis2->setTickLabels(false);

    connect(ui.customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui.customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui.customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui.customPlot->yAxis2, SLOT(setRange(QCPRange)));

    if (0 == cnt) {
        ui.customPlot->addGraph();
        ui.customPlot->graph(0)->setData(x, y);
        ui.customPlot->graph(0)->setName(s);

        //设置坐标轴恰好容纳本曲线
        ui.customPlot->graph(0)->rescaleAxes();
        ui.customPlot->xAxis->setLabel("x");
        ui.customPlot->yAxis->setLabel("y");
    }
    else {
        ui.customPlot->addGraph();
        ui.customPlot->graph(cnt)->setPen(QPen(Pen[cnt]));
        ui.customPlot->graph(cnt)->setData(x, y);
        ui.customPlot->graph(cnt)->setName(s);
        ui.customPlot->graph(cnt)->rescaleAxes(true);
    }

    // 显示图例
    ui.customPlot->legend->setVisible(true); 

    ui.customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
        QCP::iSelectLegend | QCP::iSelectPlottables);

    // 重画图像
    ui.customPlot->replot();
}