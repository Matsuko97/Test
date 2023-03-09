#include "Include.h"

QVector<QCPScatterStyle::ScatterShape> shapes = { QCPScatterStyle::ssCross, QCPScatterStyle::ssPlus,
    QCPScatterStyle::ssCircle, QCPScatterStyle::ssDisc, QCPScatterStyle::ssSquare,
    QCPScatterStyle::ssDiamond, QCPScatterStyle::ssStar, QCPScatterStyle::ssTriangle };

CustomDialog::CustomDialog(QWidget* parent, int Pen, QColor Color) :
    QColorDialog(parent)
{
    this->setObjectName(QString::fromUtf8("Custom Setting"));

    pen = Pen;
    line = 0;
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
    penStyle->addItem("SolidLine");
    penStyle->addItem("DashLine");
    penStyle->addItem("DotLine");

    QLabel* penLabel = new QLabel(penBox);
    penLabel->setObjectName(QString::fromUtf8("Pen Style :"));
    penLayout->addWidget(penLabel);
    penLayout->addWidget(penStyle);
    mainLayout->addWidget(penBox);
    //line
    QGroupBox* lineBox = new QGroupBox(centralwidget);
    QHBoxLayout* lineLayout = new QHBoxLayout(lineBox);

    lineStyle = new QComboBox(lineBox);
    QStringList lineNames;
    lineNames << "lsNone" << "lsLine";
    lineStyle->addItems(lineNames);

    QLabel* lineLabel = new QLabel(lineBox);
    lineLabel->setObjectName(QString::fromUtf8("line Style :"));
    lineLayout->addWidget(lineLabel);
    lineLayout->addWidget(lineStyle);
    mainLayout->addWidget(lineBox);
    //scatter
    QGroupBox* scatterBox = new QGroupBox(centralwidget);
    QHBoxLayout* scatterLayout = new QHBoxLayout(scatterBox);

    scatterStyle = new QComboBox(scatterBox);
    QStringList scatterNames;
    scatterNames << "ssCross" << "ssPlus" << "ssCircle" << "ssDisc" << "ssSquare" << "ssDiamond" << "ssStar" << "ssTriangle";
    scatterStyle->addItems(scatterNames);

    QLabel* scatterLabel = new QLabel(scatterBox);
    scatterLabel->setObjectName(QString::fromUtf8("Scatter Style :"));
    scatterLayout->addWidget(scatterLabel);
    scatterLayout->addWidget(scatterStyle);
    mainLayout->addWidget(scatterBox);

    colorDialog = new QColorDialog(centralwidget);
    colorDialog->setCurrentColor(color);
    mainLayout->addWidget(colorDialog);

    connect(penStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomDialog::comboBoxIndexChanged);
    connect(lineStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomDialog::comboBoxIndexChanged);
    connect(scatterStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CustomDialog::comboBoxIndexChanged);
}

void CustomDialog::comboBoxIndexChanged(int index) {
    QComboBox* combo = qobject_cast<QComboBox*>(sender());

    if (combo == penStyle) {
        pen = index + 1;
    }
    else if (combo == lineStyle) {
        line = index;
        lineFlag = true;
    }
    else {
        scatter = index;
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
            QColor color = qPen.color();
            m_pColor = new CustomDialog(this, (int)qPen.style(), qPen.color());
            m_pColor->setWindowModality(Qt::ApplicationModal);
            m_pColor->setCurrentColor(color);//初始颜色
            m_pColor->show();
            m_pColor->move(720, 200);
            //connect(m_pColor, &CustomDialog::currentColorChanged, this, &PlotWindow::ShowColor);//显示当前选中颜色的效果
            //connect(m_pColor, &CustomDialog::colorSelected, this, &PlotWindow::Set);//OK信号连接
            connect(m_pColor, SIGNAL(currentColorChanged(QColor)), this, SLOT(ShowColor(QColor)));//显示当前选中颜色的效果
            connect(m_pColor, SIGNAL(colorSelected(QColor)), this, SLOT(Set(QColor)));//OK信号连接
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
            graph->setPen(QPen(Qt::PenStyle(m_pColor->pen)));

            if (m_pColor->lineFlag) {
                ui.customPlot->graph(i)->setLineStyle((QCPGraph::LineStyle)m_pColor->line);
            }
            else {
            }

            if (m_pColor->scatterFlag) {
                ui.customPlot->graph(i)->setScatterStyle(QCPScatterStyle(shapes.at(m_pColor->scatter)));
            }
            else {}

            graph->setPen(QPen(color));
        }
    }

    m_pColor = nullptr;

    ui.customPlot->replot();
}