#include "Include.h"

Test::Test(QWidget *parent)
    : QMainWindow(parent),
    subWindowCount(0)
{
    ui.setupUi(this);
    ui.mdiArea->setViewMode(QMdiArea::SubWindowView);

    dataManager = new DataManager();
    dataSmooth = nullptr;
    instrument = new Instrument();
    dataParams = new DataReductionParam();
    adsorbate = new AdsorbateParameters();
    serialPort = new SerialPort();
    sidebar = new SideBar();
    sidebar->setParent(this);

    connect(this, &Test::dataReady, this, &Test::DrawPlot);

    connect(ui.actionInstrument, &QAction::triggered, [=](bool trigger) {
        instrument->show();
        });

    connect(ui.actionDataParam, &QAction::triggered, [=](bool trigger) {
        dataParams->show();
        });

    connect(ui.actionAdsorbates, &QAction::triggered, [=](bool trigger) {
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
        QPoint globalPos = this->mapToGlobal(QPoint(0, 0));//父窗口绝对坐标
        int x = globalPos.x();//x坐标
        int y = globalPos.y() + ui.MENU->height();//y坐标
        sidebar->setGeometry(x+2, y+2, ui.centralWidget->width() / 6, ui.centralWidget->height() - 4);
        sidebar->show();
    }

    connect(ui.actionClose_All, SIGNAL(triggered()), this, SLOT(OnCloseAll()));

    showMaximized();
}

Test::~Test()
{
    ui.mdiArea->closeAllSubWindows();

    delete dataManager;
    dataManager = nullptr;
    delete instrument;
    instrument = nullptr;
    delete dataParams;
    dataParams = nullptr;
    delete serialPort;
    serialPort = nullptr;
    delete sidebar;
    sidebar = nullptr;
}

void Test::FileOpen() {
    QString curPath = QDir::currentPath();//获取系统当前目录
    QString filter = QString::fromLocal8Bit("文本文件(*.txt);;所有文件(*.*)"); //文件过滤器

    QDir dir = QDir::current();
    dataManager->filename = dir.relativeFilePath(QFileDialog::getOpenFileName(this, tr("Open"), curPath, filter));

    if (!ReadData(dataManager->filename)) {
        QString dlgTitle = "Error";
        QString strInfo = QString::fromLocal8Bit("文件打开失败");
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
        //RegExp(str, dataManager->oriData[i].x, dataManager->oriData[i].y); //使用正则表达式
    }
    dataManager->CalcExponent(dataManager->oriData[0].y);

    if (dataManager->oriData) {
        QMessageBox::about(this, tr("Success"), QString::fromLocal8Bit("数据读取成功"));
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
    while ((pos = rx.indexIn(str, pos)) != -1)
    {
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
    QString current_date = current_date_time.toString("_yyyy_MM_dd_hh")+ QString::fromLocal8Bit("时") +
        current_date_time.toString("mm") + QString::fromLocal8Bit("分") + 
        current_date_time.toString("ss") + QString::fromLocal8Bit("秒");
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

void Test::SaveFile() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"), "", tr("Curve TagName Files (*.TXT)"));
    if (!fileName.isEmpty())
    {
        //一些处理工作，写数据到文件中
        //QList<QwtPlotCurve*>& plotCurves = m_plot->getPlotCurves();
        QFile file(fileName);
        if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
            QMessageBox::critical(this, tr("Error"), tr("Failed to open file \"%1\" for save!").arg(fileName), QMessageBox::Ok);
        //write file
        QTextStream wr(&file);
        /*for (int i = 0; i < plotCurves.count(); ++i)
        {
            wr << plotCurves[i]->title().text() << "\n";
        }*/
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
    }

    PlotWindow* plot = new PlotWindow();
    QMdiSubWindow* s = nullptr;

    s = ui.mdiArea->addSubWindow(plot);

    if (ui.actionSideBar->isChecked()) {
        s->setGeometry(this->width() / 6, 0, this->width() * 5 / 6 - 5, this->height() - ui.MENU->height() - 30);
    }
    else {
        s->setGeometry(0, 0, this->width()-5, this->height() - ui.MENU->height() - 30);
    }
    plot->show();
}

void Test::ShowSideBar() {
    if (ui.actionSideBar->isChecked()) {
        sidebar->show();
    }
    else {
        sidebar->hide();
    }
    this->resize(this->size() - QSize(1, 1));
    this->resize(this->size() + QSize(1, 1));
}

void Test::resizeEvent(QResizeEvent* event) {
    if (ui.mdiArea->subWindowList().count() > 0){
        emit plotWindowResize();
    }

    if (nullptr != sidebar && ui.actionSideBar->isChecked()) {
        sidebar->resize(ui.centralWidget->width() / 6, ui.centralWidget->height()-4);
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
        QMessageBox::critical(this, tr("Error"), QString::fromLocal8Bit("未读取原始数据"));
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
        QMessageBox::critical(this, tr("Error"), QString::fromLocal8Bit("未读取原始数据"));
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
        QMessageBox::critical(this, tr("Error"), QString::fromLocal8Bit("未读取原始数据"));
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