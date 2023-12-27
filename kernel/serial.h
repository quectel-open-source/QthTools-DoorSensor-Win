#ifndef SERIAL_H
#define SERIAL_H
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <QTimer>

typedef struct
{
    uint8_t *buf;
    uint16_t offset;
    uint16_t bufLen;
    uint8_t lastData;
}localRecvCache_t;

class qSerial : public QSerialPort, QSerialPortInfo
{
    Q_OBJECT

public:
    qSerial();
    ~qSerial();
    QSerialPort serial;
    void portListPeriodSet(int32_t period);
    bool serialOpen(QString portName,int baundrate,int dataBits,int parity,int stopBits,int control);
    void SerialClose();
    bool serialIsOpen();
    qint64 SerialSend(QByteArray data);
    QString getCurrentSerialName();
private:
    QTimer portGetTimer;
    QList<QString> oldPortList;
    bool isFirst;
private slots:
    void portListGetHandle(void);
    void dataReadHandle();

signals:
    void portListNoticSignal(QList<QString>);
    void dataReadNoticSignal(QByteArray);
};
#endif // GIZSERIAL_H
