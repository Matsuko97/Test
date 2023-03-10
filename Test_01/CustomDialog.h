#pragma once

#include <QDialog>
#include <QColorDialog>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

class CustomDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CustomDialog(QWidget* parent, int Pen, QColor Color, int Line);

public:
    QComboBox* penStyle;
    QComboBox* lineStyle;
    QComboBox* scatterStyle;
    QColorDialog* colorDialog;

    int pen;
    int line;
    bool lineFlag;
    int scatter;
    bool scatterFlag;
    QColor color;

signals:
    void closedSignal();

private slots:
    void comboBoxIndexChanged(int index);
};