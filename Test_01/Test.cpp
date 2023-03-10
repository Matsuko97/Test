#include "Include.h"

Test::Test(QWidget *parent)
    : QMainWindow(parent),
    subWindowCount(0)
{
    ui.setupUi(this);
    ui.mdiArea->setViewMode(QMdiArea::SubWindowView);

    dataManager = new DataManager();
    dataSmooth = nullptr;
    instrument = nullptr;
    dataParams = nullptr;
    adsorbate = nullptr;
    serialPort = new SerialPort();
    sidebar = new SideBar();
    sidebar->setParent(this);

    connect(this, &Test::dataReady, this, &Test::DrawPlot);

    connect(ui.actionInstrument, &QAction::triggered, [=](bool trigger) {
        instrument = new Instrument();
        instrument->show();
        });

    connect(ui.actionDataParam, &QAction::triggered, [=](bool trigger) {
        dataParams = new DataReductionParam();
        dataParams->show();
        });

    connect(ui.actionAdsorbates, &QAction::triggered, [=](bool trigger) {
        adsorbate = new AdsorbateParameters();
        adsorbate->show();
        });

    connect(ui.actionMethod, SIGNAL(triggered()), this, SLOT(OnCalculation()));

    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(ui.actionAverage_Filtering, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(ui.actionAverage_Filtering, 1);
    connect(ui.actionDebounce_Filtering, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(ui.actionDebounce_Filtering, 2);
    connect(ui.actionLimited_Amplitude_Filtering, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(ui.actionLimited_Amplitude_Filtering, 3);
    connect(ui.actionRecursive_Median_Filtering, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(ui.actionRecursive_Median_Filtering, 4);
    connect(ui.actionSavizkg_Golag_Smooth, SIGNAL(triggered()), signalMapper, SLOT(map()));
    signalMapper->setMapping(ui.actionSavizkg_Golag_Smooth, 5);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(OnFiltering(int)));

    connect(ui.actionSymmetric_Zero_Area_Algorithm, SIGNAL(triggered()), this, SLOT(OnPeakFinding()));
    connect(ui.actionTrend_Accumulation, SIGNAL(triggered()), this, SLOT(OnPeakFinding()));

    connect(ui.actionStraight_Line, &QAction::triggered, this, [=](bool trigger) {
            if (dataManager->fileBase != "" && ui.mdiArea->subWindowList().count() > 0) {
                int count = 0;
                Data* data = ReadDataForPlot(dataManager->fileBase, count);
                emit dataReady(data, count, "Base Line");
            }
        });
    connect(ui.actionSNIP, &QAction::triggered, this, &Test::OnBaseLine);

    connect(ui.actionSerial_Port, &QAction::triggered, [=](bool trigger) {
        serialPort->show();
        });

    connect(ui.actionPlot, SIGNAL(triggered()), this, SLOT(ShowPlotWindow()));
    connect(this, SIGNAL(plotWindowResize()), this, SLOT(OnPlotWindowResize()));
    
    connect(ui.actionSideBar, SIGNAL(triggered()), this, SLOT(ShowSideBar()));
    if (ui.actionSideBar->isChecked()) {
        //QPoint globalPos = this->mapToGlobal(QPoint(0, 0));//??????????????
        //int x = globalPos.x();//x????
        //int y = globalPos.y() + ui.MENU->height();//y????
        //QSize mainWindowSize = ui.centralWidget->size();
        //sidebar->setGeometry(x + 2, y + 2, mainWindowSize.width() / 6, mainWindowSize.height() - 4);
        sidebar->setGeometry(0, ui.MENU->height() + 1, this->width() / 6, this->height() - ui.MENU->height() - 30);
        sidebar->show();
    }

    connect(ui.actionClose_All, SIGNAL(triggered()), this, SLOT(OnCloseAll()));

    showMaximized();
}

Test::~Test() {
    ui.mdiArea->closeAllSubWindows();

    delete dataManager;
    dataManager = nullptr;

    if (instrument) {
        delete instrument;
        instrument = nullptr;
    }
    if (dataParams) {
        delete dataParams;
        dataParams = nullptr;
    }
    if (serialPort) {
        delete serialPort;
        serialPort = nullptr;
    }
    delete sidebar;
    sidebar = nullptr;
}

void Test::FileOpen() {
    QString curPath = QDir::currentPath();//????????????????
    QString filter = QString::fromLocal8Bit("????????(*.txt);;????????(*.*)"); //??????????

    QDir dir = QDir::current();
    dataManager->filename = dir.relativeFilePath(QFileDialog::getOpenFileName(this, tr("Open"), curPath, filter));

    if (!ReadData(dataManager->filename)) {
        QString dlgTitle = "Error";
        QString strInfo = QString::fromLocal8Bit("????????????");
        QMessageBox::critical(this, dlgTitle, strInfo);
    }

    return;
}

bool Test::ReadData(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    int count = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        count++;
    }
    dataManager->NumberOfData = count;
    dataManager->oriData = (Data*)calloc(dataManager->NumberOfData, sizeof(Data));
    file.close();
    
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    for (int i = 0; i < dataManager->NumberOfData; i++){
        QByteArray line = file.readLine();
        QString str(line);
        char* text = NULL;
        text = line.data();
        sscanf(text, "%lf %lf\n", &dataManager->oriData[i].x, &dataManager->oriData[i].y);
        //RegExp(str, dataManager->oriData[i].x, dataManager->oriData[i].y); //??????????????
    }
    dataManager->CalcExponent(dataManager->oriData[0].y);

    if (dataManager->oriData) {
        QMessageBox::about(this, tr("Success"), QString::fromLocal8Bit("????????????"));
    }

    file.close();

    if (ui.mdiArea->subWindowList().count() > 0)
        emit dataReady(dataManager->oriData, dataManager->NumberOfData, "origin data");

    return true;
}

void Test::RegExp(QString& str, double& data1, double& data2) {
    QRegExp rx("^\s*([+-]?\d+\.?\d*)\s+([+-]?\d+\.?\d*)\s*$");
    rx.setMinimal(false);

    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        pos += rx.matchedLength();
        QString temp = rx.cap(1);
        data1 = temp.toDouble();
        data2 = rx.cap(2).toDouble();
    }
}

QString Test::GenerateFileName(QString filename, QString type) {
    int length = filename.size();
    QString file = filename.mid(0, length - 4);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("_yyyy_MM_dd_hh")+ QString::fromLocal8Bit("??") +
        current_date_time.toString("mm") + QString::fromLocal8Bit("??") + 
        current_date_time.toString("ss") + QString::fromLocal8Bit("??");
    QString newFilename = file + "_" + type + current_date + ".txt";
    return newFilename;
}

bool Test::WriteData(QString filename, int num, Data* data) {
    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.seek(file.size());

        for (int i = 0; i < num; ++i) {
            QString str = QString("%1 %2").arg(data[i].x, 0, 'f', 3).arg(data[i].y, 0, 'f', 3);
            stream << str << endl;
        }
        file.close();
        return true;
    }
    return false;
}

void Test::SaveFile(QCPGraph* graph) {
    QStringList filters;
    filters << "Curve TagName Files (*.TXT)"
        << "Curve TagName Files (*.CSV)";

    QString selectedFilter;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), "", filters.join(";;"), &selectedFilter);
    if (!fileName.isEmpty()) {
        //????????????????????????????
        QFile file(fileName);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
            QMessageBox::critical(this, tr("Error"), tr("Failed to open file \"%1\" for save!").arg(fileName), QMessageBox::Ok);

        QTextStream out(&file);

        if (selectedFilter == filters.at(0)) {
            QCPDataContainer<QCPGraphData> data = *graph->data();
            QCPDataContainer<QCPGraphData>::iterator it;
            for (it = data.begin(); it != data.end(); ++it) {
                QString str = QString("%1 %2").arg(it->key, 0, 'f', 3).arg(it->value, 0, 'f', 3);
                out << str << endl;
            }
        }
        else if (selectedFilter == filters.at(1)) {
            QCPDataContainer<QCPGraphData> data = *graph->data();
            QCPDataContainer<QCPGraphData>::iterator it;
            for (it = data.begin(); it != data.end(); ++it) {
                QString str = QString("%1,%2").arg(it->key, 0, 'f', 3).arg(it->value, 0, 'f', 3);
                out << str << endl;
            }
        }
        // Write curve data to file
        file.close();
    }
    else
        return;
}

void Test::ShowPlotWindow() {
    int n = ui.mdiArea->subWindowList().count();
    if (n >= 10)
        return;

    subWindowCount = n + 1;

    if (n > 1) {
        //ui.mdiArea->setViewMode(QMdiArea::TabbedView);
    }

    PlotWindow* plot = new PlotWindow();
    QMdiSubWindow* s = nullptr;

    plot->ui.customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    // ????QCustomPlot??customContextMenuRequested??????????????????
    connect(plot->ui.customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    s = ui.mdiArea->addSubWindow(plot);

    QRect rect = ui.widget_2->geometry();
    s->setGeometry(0, rect.y(), rect.width() - 4, rect.height() - 4);

    plot->show();
}

PlotWindow* Test::showPlot() {
    int n = ui.mdiArea->subWindowList().count();
    if (n >= 10)
        return nullptr;

    subWindowCount = n + 1;

    if (n > 1) {
    }

    PlotWindow* plot = new PlotWindow();
    QMdiSubWindow* s = nullptr;

    plot->ui.customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    // ????QCustomPlot??customContextMenuRequested??????????????????
    connect(plot->ui.customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    s = ui.mdiArea->addSubWindow(plot);

    QRect rect = ui.widget_2->geometry();
    s->setGeometry(0, rect.y(), rect.width() - 4, rect.height() - 4);
    //if (ui.actionSideBar->isChecked()) {
    //    s->setGeometry(this->width() / 6, 0, this->width() * 5 / 6 - 5, this->height() - ui.MENU->height() - 30);
    //}
    //else {
    //    s->setGeometry(0, 0, this->width() - 5, this->height() - ui.MENU->height() - 30);
    //}
    plot->show();
    return plot;
}

void Test::showContextMenu(QPoint pos) {
    // ????????????????????????QCPAbstractPlottable????????????????????
    QMdiSubWindow* subWindow = ui.mdiArea->currentSubWindow();
    if (!subWindow)
        return;

    PlotWindow* plot = dynamic_cast<PlotWindow*>(subWindow->widget());
    if (!plot)
        return;

    QCPAbstractPlottable* plottable = plot->ui.customPlot->plottableAt(pos);
    if (!plottable)
        return;

    QMenu contextMenu(this);
    QAction* showAction = contextMenu.addAction(QString::fromLocal8Bit("??????????????"));
    connect(showAction, SIGNAL(triggered()), this, SLOT(ShowInNewPlot()));

    contextMenu.addAction(QString::fromLocal8Bit("????????"), this, [=]() {
        QList<QCPGraph*> selectedGraphs = plot->ui.customPlot->selectedGraphs();
        if (selectedGraphs.size() == 0 || selectedGraphs.size() != 1) {
            QMessageBox::warning(this, tr("Warning"), QString::fromLocal8Bit("????????????????"));
            return;
        }
        else {
            QCPGraph* selectedGraph = selectedGraphs.at(0);
            emit plot->curveSelected(selectedGraph);

            connect(plot, &PlotWindow::curveSelected, this, [=](QCPGraph* graph) {
                SaveFile(graph);
                });
        }
        });
    contextMenu.exec(plot->ui.customPlot->mapToGlobal(pos));
}

void Test::ShowSideBar() {
    if (ui.actionSideBar->isChecked()) {
        sidebar->show();
        ui.horizontalLayout->setStretch(0, 1);
        ui.horizontalLayout->setStretch(1, 5);
    }
    else {
        sidebar->hide();
        ui.horizontalLayout->setStretch(0, 0);
        ui.horizontalLayout->setStretch(1, 6);
    }
    this->resize(this->size() - QSize(1, 1));
    this->resize(this->size() + QSize(1, 1));
}

void Test::resizeEvent(QResizeEvent* event) {
    if (ui.mdiArea->subWindowList().count() > 0){
        emit plotWindowResize();
    }

    if (nullptr != sidebar && ui.actionSideBar->isChecked()) {
        sidebar->resize(ui.centralWidget->width() / 6 + 3, ui.centralWidget->height() - 2);
    }
    else{
    }
}

void Test::OnPlotWindowResize() {
    QMdiSubWindow* s = ui.mdiArea->activeSubWindow();
    if (ui.actionSideBar->isChecked()) {
        s->setGeometry(ui.centralWidget->width() / 6, 0, ui.centralWidget->width() * 5 / 6 - 5, ui.centralWidget->height() - 4);
    }
    else {
        s->setGeometry(0, 0, ui.centralWidget->width() - 5, ui.centralWidget->height() - 4);
    }
}

void Test::OnCloseAll() {
    ui.mdiArea->closeAllSubWindows();
    if (ui.actionSideBar->isChecked()) {
        sidebar->hide();
        ui.actionSideBar->setChecked(false);
    }
}

void Test::OnCalculation() {
    Calculation* calc = new Calculation();

    delete calc;
}

void Test::OnFiltering(int i) {
    if (dataManager->oriData == nullptr) {
        QMessageBox::critical(this, tr("Error"), QString::fromLocal8Bit("??????????????"));
        return;
    }

    Filtering* filtering = nullptr;

    switch (i) {
    case 1:
        filtering = new Filtering("Average_Filter");
        if (filtering->AverageFilter(dataManager) == 0) {
            QString file = GenerateFileName(dataManager->filename, filtering->type);
            dataManager->fileFilter = file;
            WriteData(file, filtering->dataManager->NumberOfData, filtering->dataManager->oriData);
        }
        break;

    case 2:
        filtering = new Filtering("Debounce_Filter");
        if (filtering->DebounceFilter(dataManager) == 0) {
            QString file = GenerateFileName(dataManager->filename, filtering->type);
            dataManager->fileFilter = file;
            WriteData(file, filtering->dataManager->NumberOfData, filtering->dataManager->oriData);
        }
        break;

    case 3:
        filtering = new Filtering("Limitede_Amplitude_Filter");
        if (filtering->LimitedAmplitudeFilter(dataManager) == 0) {
            QString file = GenerateFileName(dataManager->filename, filtering->type);
            dataManager->fileFilter = file;
            WriteData(file, filtering->dataManager->NumberOfData, filtering->dataManager->oriData);
        }
        break;

    case 4:
        filtering = new Filtering("Recursive_Median_Filter");
        if (filtering->RecursiveMedianFilter(dataManager) == 0) {
            QString file = GenerateFileName(dataManager->filename, filtering->type);
            dataManager->fileFilter = file;
            WriteData(file, filtering->dataManager->NumberOfData, filtering->dataManager->oriData);
        }
        break;

    case 5:
        filtering = new Filtering("S-G_Smooth");
        filtering->Smooth(dataManager);
        QString file = GenerateFileName(dataManager->filename, filtering->type);
        dataManager->fileFilter = file;
        dataSmooth = new DataManager(*filtering->dataManager);
        WriteData(file, filtering->dataManager->NumberOfData, filtering->dataManager->oriData);
        break;
    }

    if (filtering && ui.mdiArea->subWindowList().count() > 0)
        emit dataReady(filtering->dataManager->oriData, filtering->dataManager->NumberOfData, filtering->type);

    if (filtering) {
        delete filtering;
    }

    filtering = nullptr;
    return;
}

void Test::OnPeakFinding() {
    if (dataManager->oriData == nullptr) {
        QMessageBox::critical(this, tr("Error"), QString::fromLocal8Bit("??????????????"));
        return;
    }

    PeakFinding* peakFinding = nullptr;
    DataManager* data = dataSmooth == nullptr ? dataManager : dataSmooth;

    QAction* action = dynamic_cast<QAction*>(sender());

    if (action == ui.actionSymmetric_Zero_Area_Algorithm) {
        peakFinding = new PeakFinding("Symmetric_Zero_Area");
        data->filePeak = GenerateFileName(data->filename, peakFinding->type);
        data->fileBase = GenerateFileName(data->filename, "Baseline");
        peakFinding->SymmetricZeroArea(data);

        dataManager->Peaks = peakFinding->Peaks->CopyNode();
        if (dataSmooth != nullptr) {
            dataSmooth->Peaks = peakFinding->Peaks->CopyNode();
        }
    }
    else {
        peakFinding = new PeakFinding("Trend_Accumulation");

        data->filePeak = GenerateFileName(data->filename, peakFinding->type);

        peakFinding->TrendAccumulation(data);
    }

    dataManager->filePeak = data->filePeak;
    dataManager->fileBase = data->fileBase;
    if (dataSmooth != nullptr) {
        dataSmooth->filePeak = data->filePeak;
        dataSmooth->fileBase = data->fileBase;
    }

    if (peakFinding && ui.mdiArea->subWindowList().count() > 0){
        int count = 0;
        Data* dataDraw = ReadDataForPlot(data->filePeak, count);
        emit dataReady(dataDraw, count, peakFinding->type);
    }

    if (peakFinding) {
        delete peakFinding;
    }

    return;
}

void Test::DrawPlot(Data* data, int n, QString s) {
    PlotWindow* p;
    if (ui.mdiArea->subWindowList().count() > 0) {
        p = (PlotWindow*)ui.mdiArea->currentSubWindow()->widget();
        p->drawPlot(data, n, s);
    }
}

Data* Test::ReadDataForPlot(QString filename, int& count) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        count++;
    }
    Data* data = (Data*)calloc(count, sizeof(Data));
    file.close();

    file.open(QIODevice::ReadOnly | QIODevice::Text);
    for (int i = 0; i < count; i++) {
        QByteArray line = file.readLine();
        QString str(line);
        char* text = NULL;
        text = line.data();
        sscanf(text, "%lf %lf\n", &data[i].x, &data[i].y);
    }

    file.close();

    return data;
}

void Test::OnBaseLine() {
    if (dataManager->oriData == nullptr) {
        QMessageBox::critical(this, tr("Error"), QString::fromLocal8Bit("??????????????"));
        return;
    }

    PeakFinding* peakFinding = nullptr;
    DataManager* data = dataSmooth == nullptr ? dataManager : dataSmooth;
    
    peakFinding = new PeakFinding("SNIP");

    data->fileSNIP = GenerateFileName(data->filename, peakFinding->type);

    peakFinding->ImprovedSNIP(data, data->Peaks);

    if (peakFinding && ui.mdiArea->subWindowList().count() > 0) {
        int count = 0;
        Data* dataDraw = ReadDataForPlot(data->fileSNIP, count);
        emit dataReady(dataDraw, count, peakFinding->type);
    }

    if (peakFinding) {
        delete peakFinding;
    }

    return;
}

void Test::ShowInNewPlot() {
    //QAction* action = qobject_cast<QAction*>(sender());
    //if (!action)
    //    return;

    //// ????????????????????????
    //QMenu* menu = qobject_cast<QMenu*>(action->parent());
    //if (!menu)
    //    return;

    //// ??????????????????????
    //PlotWindow* widget = (PlotWindow*)menu->parentWidget();
    //if (!widget)
    //    return;

    QMdiSubWindow* subWindow = ui.mdiArea->currentSubWindow();
    if (!subWindow)
        return;
    
    PlotWindow* oldPlot = dynamic_cast<PlotWindow*>(subWindow->widget());

    QCustomPlot* plot = oldPlot->ui.customPlot;
    if (!plot)
        return;

    QList<QCPGraph*> selectedGraphs = plot->selectedGraphs();
    if (selectedGraphs.size() == 0 || selectedGraphs.size() != 1) {
        QMessageBox::warning(this, tr("Warning"), QString::fromLocal8Bit("????????????????"));
        return;
    }

    PlotWindow* newPlot = showPlot();
    if (newPlot == nullptr)
        return;

    QCPGraph* selectedGraph = selectedGraphs.at(0);
    QSharedPointer<QCPGraphDataContainer> data = selectedGraph->data();
    // ???? dataContainer ????????????

    QCustomPlot* otherCustomPlot = newPlot->ui.customPlot;
    QCPGraph* graphInOtherPlot = otherCustomPlot->addGraph();
    graphInOtherPlot->setData(data);

    otherCustomPlot->xAxis2->setVisible(true);
    otherCustomPlot->xAxis2->setTickLabels(false);
    otherCustomPlot->yAxis2->setVisible(true);
    otherCustomPlot->yAxis2->setTickLabels(false);

    connect(otherCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), otherCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(otherCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), otherCustomPlot->yAxis2, SLOT(setRange(QCPRange)));
    otherCustomPlot->legend->setVisible(true);
    otherCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
        QCP::iSelectLegend | QCP::iSelectPlottables);

    otherCustomPlot->rescaleAxes();
    otherCustomPlot->replot();
}