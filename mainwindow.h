#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "app/userinfo.h"
#include "userConfig.h"
#include "app/appConfig.h"
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void recvUserInfo(QStringList list);
    void recvOpenDeviceSuccess(QStringList list);
    void recvDeviceErrorStatus(QStringList list);
    void recvWidgetInfoChange(int widgetNum,QStringList list);
    void recvSerialRecvData(QByteArray data);
    void pushButtonOperate();
    void recvErrorDialogSelect(int mode);
    void recvErrorDialogSelect();
    void waitAckTimeout();
    void recvResultDialog(int mode,QStringList list);

    void on_pushButton_check_clicked();

    void on_commandLinkButton_config_clicked();

    void on_commandLinkButton_checkHistory_clicked();

    void on_checkBox_serialShow_clicked(bool checked);

    void on_pushButton_barCodeFold_clicked();

    void on_pushButton_barCodeFold2_clicked();

    void on_pushButton_barCodeFold3_clicked();

    void on_comboBox_barCodeSeclect_currentIndexChanged(const QString &arg1);

    void action_handle(QAction *action);
private:
    void uiAttributeSet(void);
    void initInfo();
    void setCheckEnableStatus(bool status);
    void setTableWidgetItems(int sortMode,QWidget *widget,int startPlace,int otherPlace,QStringList dataList);
    void setPushButtonFont(QWidget *widget,int mode);
    void GenerateQRcode(QString tempstr, QLabel *label);
//    void oneQRcode(QString tempstr,QLabel *label);
    void pushButton_resultShow(QWidget *widget,int result);
    void operateNextEvent();
    void saveMessageBoxShow();
    void setPushButtonStatus(int mode,int rowLine=-1);
    void resetPushButtonStatus();
    void resetLaberShowBarcode();
    void resetCheck();
    void communiteConfigWidgetShow();
    bool sendProtocolData(int QPushButtonNum);
    void DialogOperateErrorCreate();
    void qrcodeShowRules(int labelNumber,bool flag);
    void changeQrcodeWidget(int changeWidget,bool flag);
    void changeQrcodeWidgetEx(int changeWidget,QList<int>newList);
    QString expendShowData(QString data,int mode = 0);
    void logTextEditDisplay(QString data);
    void sendSerialCheckEnd();
    QString getTableWidgetItemResult(int row);
private:
    Ui::MainWindow *ui;
    QWidget *m_userInfoFunc = NULL;
    QWidget *m_comuniteConfigureFunc = NULL;
    QWidget *m_saveRecordFunc = NULL;
    QTimer *serialAckTimer = NULL;
    QDialog *currentDialog = NULL;
    userConfig *m_userConfig = NULL;
    bool isChecking = false;  // 下一轮测试 暂不做此限制 2022/5/16
    bool isFirstCheck = true;
    checkStatusInfo *m_checkDeviceInfo = NULL;
    QList<int>showQrcodeList;
    QObject *updateVersion;
};
#endif // MAINWINDOW_H
