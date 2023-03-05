#pragma once

#include <QtWidgets/QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ui_SerialPort.h"

class SerialPort : public QMainWindow {
    Q_OBJECT

public:
    SerialPort(QWidget* parent = nullptr);
    ~SerialPort();

signals:    //�Զ����ź�
    //����ʹ���ź�
    void my_send_signals(bool);

private slots:  //�Զ���ۺ���
    //���ͨѶ�˿ڲۺ���
    void btn_serial_check(bool);
    //��ѡ��˿ڲۺ���
    void btn_open_port(bool);
    //�ر�ѡ��˿ڲۺ���
    void btn_close_port(bool);
    //�������ݲۺ���
    void btn_send_data(bool);
    //�������ݲۺ���
    void receive_data(void);
    //��ս��ղۺ���
    void btn_clear_rev(bool);

    //�Զ����͸�ѡ��ۺ���
    void on_checkBox_stateChanged(int arg1);

private:
    Ui::SerialPort* ui;
    /*--------funtion---------------------*/
    //�û�ϵͳ��ʼ��
    void system_init();
    //�ַ���ת16����
    QByteArray QString2Hex(QString str);
    //�ַ�ת16����
    char ConvertHexChar(char ch);
    /*--------variable--------------------*/
    //����ȫ��������
    QSerialPort global_port;
    //�Զ����Ͷ�ʱ������
    QTimer* timer;
};