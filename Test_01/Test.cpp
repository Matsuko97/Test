#include "Include.h"

Test::Test(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    //showMaximized();

    //connect(ui.actionSave, SIGNAL(triggered()), this, SLOT(SaveFile()));
}

Test::~Test()
{}

void Test::FileOpen() {
    QString curPath = QDir::currentPath();//获取系统当前目录
    QString filter = QString::fromLocal8Bit("文本文件(*.txt);;所有文件(*.*)"); //文件过滤器
    exp.filename = QFileDialog::getOpenFileName(this, tr("Open"), curPath, filter);

    if (!GetData(exp.filename)) {
        QString dlgTitle = "Error";
        QString strInfo = QString::fromLocal8Bit("文件打开失败");
        QMessageBox::critical(this, dlgTitle, strInfo);
    }

    return;
}

bool Test::GetData(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    int count = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        count++;
    }
    exp.NumberOfData = count;
    exp.oriData = (Data*)calloc(exp.NumberOfData, sizeof(Data));
    exp.xyData = (Data*)calloc(exp.NumberOfData, sizeof(Data));
    file.close();

    
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    for (int i = 0; i < exp.NumberOfData; i++){
        QByteArray line = file.readLine();
        QString str(line);
        char* text = NULL;
        text = line.data();
        sscanf(text, "%lf %lf\n", &exp.oriData[i].x, &exp.oriData[i].y);
        //RegExp(str, exp.oriData[i].x, exp.oriData[i].y); //使用正则表达式
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