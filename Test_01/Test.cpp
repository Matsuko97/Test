#include "Include.h"

Test::Test(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    dataManager = new DataManager();
    plot = new PlotWindow();
    connect(ui.actionPlot, SIGNAL(triggered()), this, SLOT(ShowPlotWindow()));

    sidebar = new SideBar();
    connect(ui.actionSideBar, SIGNAL(triggered()), this, SLOT(ShowSideBar()));
    sidebar->setParent(this);
    if (ui.actionSideBar->isChecked()) {
        //ui.verticalLayout->addWidget(sidebar);
        QPoint globalPos = this->mapToGlobal(QPoint(0, 0));//父窗口绝对坐标
        int x = globalPos.x();//x坐标
        int y = globalPos.y() + ui.MENU->height();//y坐标
        sidebar->resize(this->width()/4, this->height());
        sidebar->move(x, y);//窗口移动
        sidebar->show();
    }

    dataManager = new DataManager();
    
    connect(ui.actionMethod, SIGNAL(triggered()), this, SLOT(OnCalculation()));
    connect(ui.actionPeak_Finding_Algorithm, SIGNAL(triggered()), this, SLOT(OnCalculation()));
    
    showMaximized();
}

Test::~Test()
{
    if (plot)
        delete plot;

    if (sidebar)
        delete sidebar;

    if (dataManager)
        delete dataManager;
}

void Test::FileOpen() {
    QString curPath = QDir::currentPath();//获取系统当前目录
    QString filter = QString::fromLocal8Bit("文本文件(*.txt);;所有文件(*.*)"); //文件过滤器
    dataManager->filename = QFileDialog::getOpenFileName(this, tr("Open"), curPath, filter);

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

    file.close();
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

QString Test::GenerateFileName(QString filename) {
    int length = filename.size();
    QString file = filename.mid(0, length - 4);
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("_yyyy_MM_dd_hh")+ QString::fromLocal8Bit("时") +
        current_date_time.toString("mm") + QString::fromLocal8Bit("分") + 
        current_date_time.toString("ss") + QString::fromLocal8Bit("秒");
    QString newFilename = file + current_date + ".txt";
    return newFilename;
}

bool Test::WriteData(QString name, int num, Data* data) {
    QString filename = GenerateFileName(name);

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.seek(file.size());

        for (int i = 0; i < num; ++i) {
            QString str = QString("%1 %2").arg(data[i].x, 0, 'f', 3).arg(data[i].y, 0, 'f', 3);
            //str.asprintf("%.3lf  %.3lf\n", data[i].x, data[i].y);
            stream << str << endl;
        }
        file.close();
        return true;
    }
    return false;
}

bool Test::WriteData(QString name, PeakNode* Peaks, int* TestData, int PeaksThreshold) {
    QString filename = GenerateFileName(name);

    QFile file(filename);

    PeakNode* p = Peaks;

    if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        QTextStream stream(&file);
        stream.seek(file.size());

        while (p != NULL) {
            if (abs(TestData[p->indPeak] - TestData[p->indStart]) >= PeaksThreshold) //设置阈值，过滤掉小峰RoundRatio*80/100
            {
                QString str = QString("%1 %2").arg(dataManager->oriData[p->indStart].x, 0, 'f', 3).arg(dataManager->oriData[p->indStart].y, 0, 'f', 3);
                stream << str << endl;
                str = QString("%1 %2").arg(dataManager->oriData[p->indPeak].x, 0, 'f', 3).arg(dataManager->oriData[p->indPeak].y, 0, 'f', 3);
                stream << str << endl;
                str = QString("%1 %2").arg(dataManager->oriData[p->indEnd].x, 0, 'f', 3).arg(dataManager->oriData[p->indEnd].y, 0, 'f', 3);
                stream << str << endl;
            }
            p = p->next;
        }

        p = NULL;

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
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));//父窗口绝对坐标
    int x = globalPos.x();//x坐标
    int y = globalPos.y() + ui.MENU->height() + 40;//y坐标

    if (ui.actionSideBar->isChecked()) {
        plot->setGeometry(this->width() / 4 + x, y, this->width() * 3 / 4, this->height() - ui.MENU->height() - 40);
    }
    else {
        plot->setGeometry(x, y, this->width(), this->height() - ui.MENU->height() - 40);
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
    QPoint globalPos = this->mapToGlobal(QPoint(0, 0));//父窗口绝对坐标
    int x = globalPos.x();//x坐标
    int y = globalPos.y() + ui.MENU->height() + 40;//y坐标
    if (nullptr != sidebar && ui.actionSideBar->isChecked()) {
        sidebar->resize(ui.centralWidget->width() / 4, ui.centralWidget->height());
        plot->setGeometry(this->width() / 4 + x, y, this->width() * 3 / 4, this->height() - ui.MENU->height() - 40);
    }
    else{
        plot->setGeometry(x, y, this->width(), this->height() - ui.MENU->height() - 40);
    }
}

void Test::OnCalculation() {
    Calculation* calc = new Calculation();

    delete calc;
}

void Test::OnPeakFinding() {
    PeakFinding* peakFinding = new PeakFinding();

    delete peakFinding;
}