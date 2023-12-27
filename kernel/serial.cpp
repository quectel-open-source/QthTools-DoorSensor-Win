#include "serial.h"
#include "QByteArray"
#include "QDateTime"
#include "QDebug"

qSerial::qSerial()
{
    connect(&this->portGetTimer, SIGNAL(timeout()), this, SLOT(portListGetHandle()));
    connect(&this->serial,SIGNAL(readyRead()),this,SLOT(dataReadHandle()));
    this->isFirst = true;
}

qSerial::~qSerial()
{
    if(this->serial.isOpen())
    {
        this->serial.close();
    }
}

void qSerial::portListPeriodSet(int32_t period)
{
    qInfo()<<__FUNCTION__;
    if(this->portGetTimer.isActive())
    {
        this->portGetTimer.stop();
    }
    this->portGetTimer.start(period);
}

void qSerial::portListGetHandle(void)
{
//    qInfo()<<__FUNCTION__;
    QList<QString> portList;
    foreach(const QSerialPortInfo &info, this->availablePorts())
    {
        //AT端口放在前面
        if(info.description().indexOf("AT") >= 0)
        {
            portList.insert(0,info.portName()+"-"+info.description());
        }
        else
        {
            portList.append(info.portName()+"-"+info.description());
        }

    }
    if(this->oldPortList != portList || this->isFirst == true)
    {
        this->isFirst = false;
        this->oldPortList = portList;
        emit portListNoticSignal(portList);
    }
}

void qSerial::dataReadHandle()
{
    qInfo()<<__FUNCTION__;
    QByteArray readData = this->serial.readAll();
    if(readData.isEmpty()==false)
    {
        emit dataReadNoticSignal(readData);
    }
}

bool qSerial::serialOpen(QString portName,int baundrate,int dataBits,int parity,int stopBits,int control)
{
    qInfo()<<__FUNCTION__;
    bool isOpen = false;
    if(!portName.isNull())
    {
        this->serial.setPortName(portName.split("-").at(0));
        this->serial.setBaudRate(baundrate);
        this->serial.setDataBits(DataBits(dataBits));
        this->serial.setParity(Parity(parity));
        this->serial.setStopBits(StopBits(stopBits));
        this->serial.setFlowControl(FlowControl(control));
        isOpen =  this->serial.open(QIODevice::ReadWrite);
    }
    return isOpen;
}
void qSerial::SerialClose()
{
    qInfo()<<__FUNCTION__;
    if(this->serial.isOpen())
    {
        this->serial.close();
    }
}

bool qSerial::serialIsOpen()
{
    return this->serial.isOpen();
}

qint64 qSerial::SerialSend(QByteArray data)
{
    if(this->serial.isOpen())
    {
        return this->serial.write(data,data.length());
    }
    return -1;
}

QString qSerial::getCurrentSerialName()
{
    if(this->serial.isOpen())
    {
        return this->serial.portName();
    }
    return NULL;
}

