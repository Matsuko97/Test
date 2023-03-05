#include "Include.h"

SerialPort::SerialPort(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::SerialPort)
{
    ui->setupUi(this);

    /*----------user-----------*/
    //init �û�ϵͳ��ʼ��
    system_init();
}

SerialPort::~SerialPort()
{
    delete ui;
}

/*---------------------------------------------------------------------
 *             funtion
 * --------------------------------------------------------------------*/
 //�û�ϵͳ��ʼ��
void SerialPort::system_init()
{
    //port conifg
    ui->cmd_port_name->clear();
    //ͨ��QSerialPortInfo���ҿ��ô���
    foreach(const QSerialPortInfo & info, QSerialPortInfo::availablePorts())
    {
        //�����ô�����ӵ��˿���ʾ��
        ui->cmd_port_name->addItem(info.portName());
    }

    //���ô���״̬��ǩΪ��ɫ ��ʾδ����״̬
    ui->lab_status->setStyleSheet("color:red");
    //statusBar ״̬����ʾ�˿�״̬ δ����
    QString sm = "%1 CLOSED";
    QString status = sm.arg(ui->cmd_port_name->currentText());
    ui->statusBar->showMessage(status);
    ui->statusBar->setStyleSheet("color:red");

    //timer �Զ�����
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        emit my_send_signals(true); //���������ź�
        });


    //connect
    //check port ���ͨѶ�˿�
    connect(ui->btn_serial, &QPushButton::clicked, this, &SerialPort::btn_serial_check);
    //open port ��ѡ��˿�
    connect(ui->btn_open, &QPushButton::clicked, this, &SerialPort::btn_open_port);
    //close port �ر�ѡ��˿�
    connect(ui->btn_close, &QPushButton::clicked, this, &SerialPort::btn_close_port);

    //send data ���Ͱ�ť ���������ź�
    connect(ui->btn_send, &QPushButton::clicked, [=]() {
        emit my_send_signals(true);
        });
    //�����ź�   ���Ͳۺ���
    connect(this, &SerialPort::my_send_signals, this, &SerialPort::btn_send_data);

    //receive data �������ݽ����괥�����������ʾ�����ı���
    connect(&global_port, &QSerialPort::readyRead, this, &SerialPort::receive_data);
    //clear recevie ��������ı��������
    connect(ui->btn_clear, &QPushButton::clicked, this, &SerialPort::btn_clear_rev);

}

//�ַ���ת16����
QByteArray SerialPort::QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();

    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len; )
    {
        hstr = str[i].toLatin1();
        if (hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;
}
//�ַ�ת16����
char SerialPort::ConvertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else return (-1);
}

/*---------------------------------------------------------------------
 *             slots
 * --------------------------------------------------------------------*/
 //���ͨѶ�˿ڲۺ���
void SerialPort::btn_serial_check(bool)
{
    ui->cmd_port_name->clear();
    //ͨ��QSerialPortInfo���ҿ��ô���
    foreach(const QSerialPortInfo & info, QSerialPortInfo::availablePorts())
    {
        ui->cmd_port_name->addItem(info.portName());
    }
}

//��ѡ��˿ڲۺ���
void SerialPort::btn_open_port(bool)
{
    //port name ���ö˿�
    global_port.setPortName(ui->cmd_port_name->currentText());
    //baud rate ���ò�����
    global_port.setBaudRate(ui->cmd_baud_rate->currentText().toInt());
    //parity ����У��λ
    switch (ui->cmd_parity->currentIndex()) {
    case 0:     //��У��
        global_port.setParity(QSerialPort::NoParity);
        break;
    case 1:     //żУ��
        global_port.setParity(QSerialPort::EvenParity);
        break;
    default:    //��У��
        global_port.setParity(QSerialPort::OddParity);
        break;
    }

    //data bits ��������λ
    switch (ui->cmd_data_bits->currentText().toInt()) {
    case 8:
        global_port.setDataBits(QSerialPort::Data8);
        break;
    case 7:
        global_port.setDataBits(QSerialPort::Data7);
        break;
    case 6:
        global_port.setDataBits(QSerialPort::Data6);
        break;
    default:
        global_port.setDataBits(QSerialPort::Data5);
        break;
    }

    //stop bits ����ֹͣλ
    switch (ui->cmd_stop_bits->currentIndex()) {
    case 0:
        global_port.setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        global_port.setStopBits(QSerialPort::OneAndHalfStop);
        break;
    default:
        global_port.setStopBits(QSerialPort::TwoStop);
        break;
    }

    //port open ��ѡ��˿�
    bool ret = global_port.open(QIODevice::ReadWrite);

    if (ret) {
        //�򿪳ɹ�����״̬
        ui->lab_status->setText("Connected");
        //���ô���״̬��ǩΪ��ɫ ��ʾ������״̬
        ui->lab_status->setStyleSheet("color:green");

        //statusBar ״̬����ʾ�˿�״̬
        QString sm = "%1 OPENED, %2, 8, NONE, 1";
        QString status = sm.arg(global_port.portName()).arg(global_port.baudRate());
        ui->statusBar->showMessage(status);
        ui->statusBar->setStyleSheet("color:green");
    }
}

//�ر�ѡ��˿ڲۺ���
void SerialPort::btn_close_port(bool)
{

    global_port.close();

    //�رն˿ں���ʾ״̬
    ui->lab_status->setText("DisConnected");
    ui->lab_status->setStyleSheet("color:red");

    //statusBar ״̬����ʾ�˿�״̬
    QString sm = "%1 CLOSED";
    QString status = sm.arg(global_port.portName());
    ui->statusBar->showMessage(status);
    ui->statusBar->setStyleSheet("color:red");
}

//�������ݲۺ���
void SerialPort::btn_send_data(bool)
{
    QString data = ui->pte_send->toPlainText();
    QByteArray array;

    //Hex��ѡ��
    if (ui->chk_send_hex->checkState() == Qt::Checked) {
        array = QString2Hex(data);  //HEX 16����
    }
    else {
        array = data.toLatin1();    //ASCII
    }
    global_port.write(array);   //��������
}

//�������ݲۺ���
void SerialPort::receive_data(void)
{
    QByteArray array = global_port.readAll();
    QString str_rev;

    if (ui->chk_rev_hex->checkState() == Qt::Checked) {   //HEX 16����
        if (ui->chk_rev_line->checkState() == Qt::Checked) {  //�Զ�����
            if (ui->chk_rev_time->checkState() == Qt::Checked) {  //��ʾʱ��
                //��ȡ��ǰϵͳʱ��
                QDateTime nowtime = QDateTime::currentDateTime();
                //ʱ��ת��Ϊ�ַ�����ʽ
                str_rev = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
                //���Ͻ������� ת��Ϊ16���Ʋ��ո�ֿ� �������ݻ���
                str_rev += QString(array.toHex(' ').toUpper().append(' ')).append("\r\n");
            }
            else {
                str_rev = QString(array.toHex(' ').toUpper().append(' ')).append("\r\n");
            }
        }
        else {
            str_rev = QString(array.toHex(' ').toUpper().append(' '));
        }

    }
    else {
        if (ui->chk_rev_line->checkState() == Qt::Checked) {
            if (ui->chk_rev_time->checkState() == Qt::Checked) {
                QDateTime nowtime = QDateTime::currentDateTime();
                str_rev = "[" + nowtime.toString("yyyy-MM-dd hh:mm:ss") + "] ";
                str_rev += QString(array).append("\r\n");
            }
            else {
                str_rev = QString(array).append("\r\n");
            }
        }
        else {
            str_rev = QString(array);
        }

    }

    ui->pte_rcv->insertPlainText(str_rev);
}

//��ս����ı���ۺ���
void SerialPort::btn_clear_rev(bool)
{
    ui->pte_rcv->clear();
}

//�Զ�������ѡ��  ������ʱ����ֹͣ��ʱ��
void SerialPort::on_checkBox_stateChanged(int arg1)
{
    if (arg1) {
        timer->start(ui->spinBox->value()); //������ʱ��
    }
    else {
        timer->stop();  //ֹͣ��ʱ��
    }
}