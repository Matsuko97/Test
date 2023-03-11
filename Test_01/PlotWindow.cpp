#include "Include.h"

QVector<QCPScatterStyle::ScatterShape> shapes = { QCPScatterStyle::ssCross, QCPScatterStyle::ssPlus,
    QCPScatterStyle::ssCircle, QCPScatterStyle::ssDisc, QCPScatterStyle::ssSquare,
    QCPScatterStyle::ssDiamond, QCPScatterStyle::ssStar, QCPScatterStyle::ssTriangle };

CustomDialog::CustomDialog(QWidget* parent, int Pen, QColor Color, int Line) :
    QDialog(parent)
{
    pen = Pen;
    line = Line;
    lineFlag = false;
    scatter = 0;
    scatterFlag = false;
    color = Color;

    QWidget* centralwidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralwidget);

    //pen
    QGroupBox* penBox = new QGroupBox(centralwidget);
    QHBoxLayout* penLayout = new QHBoxLayout(penBox);

    penStyle = new QComboBox(penBox);
    //penStyle = new QComboBox(this);
    penStyle->addItem("Solid Line");
    penStyle->addItem("Dash Line");
    penStyle->addItem("Dot Line");
    penStyle->setCurrentIndex(pen - 1);

    QLabel* penLabel = new QLabel("Pen Style : ", penBox);
    penLayout->addWidget(penLabel);
    penLayout->addWidget(penStyle);
    mainLayout->addWidget(penBox);

    //line
    QGroupBox* lineBox = new QGroupBox(centralwidget);
    QHBoxLayout* lineLayout = new QHBoxLayout(lineBox);

    lineStyle = new QComboBox(lineBox);
    QStringList lineNames;
    lineNames << "None" << "Line";
    lineStyle->addItems(lineNames);
    lineStyle->setCurrentIndex(line < 0 ? 0 : line);

    QLabel* lineLabel = new QLabel("line Style : ", lineBox);
    lineLayout->addWidget(lineLabel);
    lineLayout->addWidget(lineStyle);
    mainLayout->addWidget(lineBox);

    //scatter
    QGroupBox* scatterBox = new QGroupBox(centralwidget);
    QHBoxLayout* scatterLayout = new QHBoxLayout(scatterBox);

    scatterStyle = new QComboBox(scatterBox);

    QStringList scatterNames;
    scatterNames << "None" << "Cross" << "Plus" << "Circle" << "Disc" << "Square" << "Diamond" << "Star" << "Triangle";
    scatterStyle->addItems(scatterNames);

    QLabel* scatterLabel = new QLabel("Scatter Style : ", scatterBox);
    scatterLayout->addWidget(scatterLabel);
    scatterLayout->addWidget(scatterStyle);
    mainLayout->addWidget(scatterBox);

    colorDialog = new QColorDialog(this);
    colorDialog->setOption(QColorDialog::ShowAlphaChannel, true);
    colorDialog->setCurrentColor(color);
    mainLayout->addWidget(colorDialog);
    setLayout(mainLayout);
    setWindowTitle("Custom Setting Dialog");

    connect(penStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomDialog::comboBoxIndexChanged);
    connect(lineStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomDialog::comboBoxIndexChanged);
    connect(scatterStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomDialog::comboBoxIndexChanged);
    connect(this, &CustomDialog::closedSignal, this, &QDialog::accept);
}

void CustomDialog::comboBoxIndexChanged(int index) {
    QComboBox* combo = qobject_cast<QComboBox*>(sender());

    if (combo == penStyle) {
        pen = index + 1;
    }
    else if (combo == lineStyle) {
        line = index == 1 ? 1 : -1;
        lineFlag = true;
    }
    else {
        scatter = index - 1;
        scatterFlag = true;
    }
}


//*********   PlotWindow   **********
PlotWindow::PlotWindow(QWidget* parent):QWidget(parent)
{
	ui.setupUi(this);

    m_pColor = nullptr;

	setAttribute(Qt::WA_QuitOnClose, false);//父窗口关闭子窗口也关闭
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Tool | Qt::NoDropShadowWindowHint | 
        Qt::WindowStaysOnTopHint);

    ui.customPlot->legend->setSelectableParts(QCPLegend::spItems);
    connect(ui.customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
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
        //ui.customPlot->graph(cnt)->setPen(QPen(Pen[cnt]));
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

//CustomDialog
void PlotWindow::selectionChanged()
{
    for (int i = 0; i < ui.customPlot->graphCount(); ++i) {
        QCPGraph* graph = ui.customPlot->graph(i);
        QCPPlottableLegendItem* item = ui.customPlot->legend->itemWithPlottable(graph);
        if (item->selected()/*||graph->selected()*/) {
            QPen qPen = ui.customPlot->graph(i)->pen();
            int Line = (int)graph->lineStyle();
            m_pColor = new CustomDialog(this, (int)qPen.style(), qPen.color(), Line);
            m_pColor->setWindowModality(Qt::ApplicationModal);
            //m_pColor->colorDialog->setCurrentColor(qPen.color());//初始颜色
            m_pColor->show();
            m_pColor->move(720, 100);
            connect(m_pColor->colorDialog, SIGNAL(currentColorChanged(QColor)), this, SLOT(ShowColor(QColor)));//显示当前选中颜色的效果
            connect(m_pColor->colorDialog, SIGNAL(colorSelected(QColor)), this, SLOT(Set(QColor)));//OK信号连接
            connect(m_pColor->colorDialog, &QColorDialog::rejected, [=]() {
                // 用户单击了“Cancel”按钮，执行相应操作
                emit m_pColor->closedSignal();
                });
        }
    }
}

void PlotWindow::ShowColor(const QColor& color)//只要当前颜色在对话框中发生改变，就会触发该信号。
{
    qDebug() << "1111" << color;
}
void PlotWindow::Set(const QColor& color)//当用户选中某一颜色并点击“OK”后，就会触发该信号。
{
    for (int i = 0; i < ui.customPlot->graphCount(); ++i) {
        QCPGraph* graph = ui.customPlot->graph(i);
        QCPPlottableLegendItem* item = ui.customPlot->legend->itemWithPlottable(graph);
        if (item->selected()/*|| graph->selected()*/) {
            QPen pen(color);
            pen.setStyle(Qt::PenStyle(m_pColor->pen));
            graph->setPen(pen);

            if (m_pColor->lineFlag) {
                ui.customPlot->graph(i)->setLineStyle((QCPGraph::LineStyle)m_pColor->line);
            }
            else {
            }

            if (m_pColor->scatterFlag) {
                ui.customPlot->graph(i)->setScatterStyle(QCPScatterStyle(shapes.at(m_pColor->scatter)));
            }
            else {}

        }
    }

    emit m_pColor->closedSignal();
    m_pColor = nullptr;

    ui.customPlot->replot();
}