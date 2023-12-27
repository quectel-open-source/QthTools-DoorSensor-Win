#ifndef COMMUNITECONFIGURE_H
#define COMMUNITECONFIGURE_H

#include <QWidget>
#include "appinfosave.h"

namespace Ui {
class communiteConfigure;
}

class communiteConfigure : public QWidget
{
    Q_OBJECT

public:
    explicit communiteConfigure(QWidget *parent = nullptr);
    ~communiteConfigure();
    void widgetShow(QRect rect);                                                  // 窗口显示
    void widgetHide();                                                  // 窗口隐藏
    bool sendProtocolBasicData(int pkid,int cmd);                       // 串口发送简易类型数据
    bool sendProtocolExpendData(int pkid, int cmd,QByteArray payload);  // 串口发送带数据域的数据
    bool printerPrintImage(QPixmap *image);                             // 打印图片
signals:
    void openDeviceSuccess(QStringList);
    void signalDeviceStatus(QStringList);
    void signalWidgetChange(int,QStringList);
    void signalSerialRecvData(QByteArray);


private slots:
    /* 打印机事件槽函数 */
    void on_pushButton_printerMoreAttribute_clicked();

    void on_pushButton_printerAttribute_clicked();

    void on_pushButton_printPageAttribute_clicked();

    void on_pushButton_printPreview_clicked();

    /* ************* */

    /* 串口事件槽函数 */
    void portListNoticSlot(QList<QString> portList);
    void dataReadNoticSlot(QByteArray data);
    void dataHandle(QByteArray &data);
    /* ************* */

    /* csq事件槽函数 */
    void on_spinBox_CSQMinValue_valueChanged(int arg1);

    void on_radioButton_setCSQRange_toggled(bool checked);

    /* ************* */

    /* 外设相关的槽函数 */
    void checkDeviceStatusTimeout();
    void recvCheckTimeOut();
    /* ************* */

    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

    void on_pushButton_configReset_clicked();

    void on_spinBox_CSQMaxValue_valueChanged(int arg1);

private:
    void uiAttributeSet(void);
    void historyRecover();
    void paintEvent(QPaintEvent *event);
    void closeEvent(QCloseEvent *event);
    void initSerial();
    void initPrinter();
    void resetSerial();
    void resetPrinter();
    bool openSerial();
    QByteArray splitDataLine(QByteArray data,int *outLen);
    bool serialDataAnalysis(QByteArray data);
    void dataMatchCombox(QWidget *widget,QString data);
private:
    Ui::communiteConfigure *ui;
    QObject *gc_qserial = NULL;
    QObject *gc_printer = NULL;

    QTimer *gc_checkDeviceStatus = NULL;
    QTimer *gc_recvTimer = NULL;

    QByteArray gc_recvData;
    appInfoSave *gc_appInfoSaveInstance = NULL;

    bool gc_serialOpenFlag = false;
    bool gc_activityFlag = false;
    bool gc_recvHandleFlag = false;
    bool isFirstUpdateSerialList = false;

    int g_csqMinValue = 0;
    int g_csqMaxValue = 0;
};

#endif // COMMUNITECONFIGURE_H
