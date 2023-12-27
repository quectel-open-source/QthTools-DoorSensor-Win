#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "app/communiteconfigure.h"
#include "externWidget/myswitchbutton.h"
#include "third/qrencode/qrencode.h"
#include <QPainter>
#include "app/dialogoperateerror.h"
#include "app/dialogtipscombinedend.h"
#include "app/dialogsetresult.h"
#include "app/dialogtotalresult.h"
#include "app/saverecord.h"
#include "kernel/serialdatahandle.h"
#include "kernel/usebarcode.h"
#ifdef USE_UPDATE_INLINE
    #include "third/QSimpleUpdater/toolupdate.h"
#endif
#include <QDateTime>

userDataStruct m_userInfo;

typedef enum
{
    SEND_TYPE=0,
    RECV_TYPR,
}showDataType;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("移远门磁产测");
    this->setWindowIcon(QIcon(":/image/quectel.ico"));
    uiAttributeSet();
    initInfo();



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::recvUserInfo(QStringList list)
{
    if(3 == list.size())
    {
        this->show();
        m_userConfig->moduleType =  QString(list[0]);
        m_userConfig->batch = QString(list[1]);
        m_userConfig->personName = QString(list[2]);
        if(m_userConfig->isFirstShow)
        {
            communiteConfigWidgetShow();
            m_userConfig->isFirstShow = false;
        }
        ui->label_personName->setText("检测人："+m_userConfig->personName);
        m_userInfoFunc->close();
        m_userInfoFunc->deleteLater();
        m_saveRecordFunc = new saveRecord();
    }
}

void MainWindow::recvOpenDeviceSuccess(QStringList list)
{
    if(list.size() >= 2)
    {
        qDebug()<<"min:"<<m_userConfig->csqMin<<"max:"<<m_userConfig->csqMax;
        m_userConfig->deviceReady = true;
        m_userConfig->csqMin = QString(list.at(0)).toInt();
        m_userConfig->csqMax = QString(list.at(1)).toInt();
        setCheckEnableStatus(true);
    }
    ((communiteConfigure *)m_comuniteConfigureFunc)->widgetHide();
}

void MainWindow::recvDeviceErrorStatus(QStringList list)
{
    if(2 == list.size())
    {
        if(!QVariant(list[0]).toBool())
        {
            // 串口错误关闭
            m_userInfo.serialStatus = false;
            setCheckEnableStatus(false);
        }
        if(!QVariant(list[1]).toBool())
        {
            // 打印机错误关闭
            m_userInfo.printerStatus = false;
            // 暂为和串口一致条件 后期可实行无打印机不输出二维码功能
            setCheckEnableStatus(false);
        }
    }
}

void MainWindow::recvWidgetInfoChange(int widgetNum, QStringList list)
{
    qDebug()<<__FUNCTION__<<",list:"<<list;
    if(serialAckTimer->isActive())
    {
        serialAckTimer->stop();
    }
    if(!isChecking)
    {
        logTextEditDisplay(expendShowData("当前未开始检测，请先点击检测按钮"));
        return;
    }
    qDebug()<<"widgetNum:"<<widgetNum;
    qDebug()<<"m_userInfo.currentEventLine:"<<m_userInfo.currentEventLine;
    qDebug()<<"m_userConfig->isNeedOperate:"<<m_userConfig->isNeedOperate;
    // 临时修改方案
    if(RESULT_CHECK_END == widgetNum )
    {
        qDebug()<<"m_userConfig->isCheckingEnd:"<<m_userConfig->isCheckingEnd;
        if(NULL !=currentDialog)
        {
            currentDialog->close();
            currentDialog->deleteLater();
            currentDialog = NULL;
        }
        if(m_userConfig->isCheckingEnd)
        {
            logTextEditDisplay(expendShowData("接收到结束产测指令反馈"));
            saveMessageBoxShow();
        }
        else
        {
            logTextEditDisplay(expendShowData("设备已退出产测模式"));
            resetCheck();
        }
        m_userConfig->isCheckingEnd = false;
        return;
    }
    else if(m_userConfig->isCheckingInit && RESULT_CHECK_INIT == widgetNum)
    {
        logTextEditDisplay(expendShowData("接收到初始化指令反馈"));
        m_userConfig->isCheckingInit = false;
        QWidget *pushButtonWidget = ui->tableWidget_operate->cellWidget(0,2);
        ((QPushButton *)pushButtonWidget)->click();
        return;
    }
    // 过滤
    else if((m_userInfo.currentEventLine != widgetNum && (m_userInfo.currentEventLine != RESULT_INTERNET_LINE && m_userInfo.currentEventLine != RESULT_CSQ_LINE))    \
            ||(NULL != currentDialog  && m_userConfig->isNeedOperate))
    {
            qDebug()<<"设备当前不在此流程，将忽略此消息,currentEventLine:"<<m_userInfo.currentEventLine<<",widgetNum:"<<widgetNum;
            serialAckTimer->start();
            return;

    }
    switch(widgetNum)
    {
        case RESULT_MODULE_INFO_LINE:
        {

            bool result = false;
            logTextEditDisplay(expendShowData(QString("接收到%1指令反馈").arg(RESULT_MESS_STRING(widgetNum)),RECV_TYPR));
            qDebug()<<"list.size:"<<list.size();
            if(4 == list.size())
            {
                // IMEI + version + pk
                qDebug()<<"RESULT_MODULE_INFO_LINE recv list:"<<list;
                result = true;
            }
            if(5 == list.size())
            {
                // IMEI + version + pk + SN  暂不做条形码

                m_checkDeviceInfo->sn = list[3];
                QString imeiBeyondSn = "https://fy-api.iotomp.com?pk="+list[2] + "&sn="+m_checkDeviceInfo->sn;
                GenerateQRcode(list[3], ui->label_barCodeShow2);
                changeQrcodeWidget(2,(0 == ui->pushButton_barCodeFold2->text().compare("收起"))?true:false);
                GenerateQRcode(imeiBeyondSn, ui->label_barCodeShow3);
                changeQrcodeWidget(3,(0 == ui->pushButton_barCodeFold3->text().compare("收起"))?true:false);
                result = true;
            }
            if(result)
            {
                m_userInfo.checkDeviceResult = true;
                m_checkDeviceInfo->IMEI = list[0];
                m_checkDeviceInfo->version = list[1];
                m_checkDeviceInfo->pk = list[2];
                GenerateQRcode(list[0], ui->label_barCodeShow);
                changeQrcodeWidget(1,(0 == ui->pushButton_barCodeFold->text().compare("收起"))?true:false);
                QTableWidgetItem *item = new QTableWidgetItem(list[1]);
                item->setTextAlignment(Qt::AlignCenter);
                ui->tableWidget_operate->setItem(RESULT_MODULE_INFO_LINE,1,item);
                pushButton_resultShow(ui->tableWidget_operate->cellWidget(RESULT_MODULE_INFO_LINE,2),result);
            }
            else
            {
                serialAckTimer->start();
            }
            break;
        }
        case RESULT_LED_LINE:
        case RESULT_BEER_LINE:
        {
            logTextEditDisplay(expendShowData(QString("接收到%1指令反馈").arg(RESULT_MESS_STRING(widgetNum)),RECV_TYPR));
            break;
        }
        case RESULT_PUSHBUTTON_LINE:
        case RESULT_REED_LINE:
        {
            if(RESULT_PUSHBUTTON_LINE == widgetNum)
            {
                m_checkDeviceInfo->pushButtonResult = RESULT_STATUS_STRING((1==QString(list[0]).toInt()?DEV_FUNC_STATUS_OK:DEV_FUNC_STATUS_ERROR));
            }
            else if(RESULT_REED_LINE == widgetNum)
            {
                m_checkDeviceInfo->reedResult = RESULT_STATUS_STRING((1==QString(list[0]).toInt()?DEV_FUNC_STATUS_OK:DEV_FUNC_STATUS_ERROR));
            }
            if(1 == list.size())
            {
                int result = QString(list[0]).toInt();
                pushButton_resultShow(ui->tableWidget_operate->cellWidget(widgetNum,2),result == 1 ? 1:0);
            }
            else
            {
                pushButton_resultShow(ui->tableWidget_operate->cellWidget(widgetNum,2),0);
            }
        }
        break;
        case RESULT_INTERNET_LINE:
            logTextEditDisplay(expendShowData(QString("接收到%1指令反馈").arg(RESULT_MESS_STRING(widgetNum)),RECV_TYPR));
            if(2 == list.size())
            {

                int internetResult = QString(list[0]).toInt();
                int csq = QString(list[1]).toInt();
                m_checkDeviceInfo->internelResult = RESULT_STATUS_STRING((1==QString(list[0]).toInt()?DEV_FUNC_STATUS_OK:DEV_FUNC_STATUS_ERROR));
                m_checkDeviceInfo->csq = csq;
                qDebug()<<"internetResult:"<<internetResult<<",csq:"<<csq<<",m_userConfig->csqMin:"<<m_userConfig->csqMin<<",m_userConfig->csqMax:"<<m_userConfig->csqMax;
                if((1 == internetResult) && (csq >= m_userConfig->csqMin && csq <= m_userConfig->csqMax))
                {
                    m_checkDeviceInfo->csqResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_OK);
                    setPushButtonStatus(3);
                    m_userInfo.currentEventLine = RESULT_CSQ_LINE;
                    if(NULL != ui->tableWidget_operate->item(RESULT_CSQ_LINE,1))
                    {
                        ui->tableWidget_operate->item(RESULT_CSQ_LINE,1)->setText(QString(list[1])+QString("  正常值(%1~%2)").arg(QString::number(m_userConfig->csqMin)).arg(QString::number(m_userConfig->csqMax)));
                    }
                    else
                    {
                        QTableWidgetItem *csqShowItem = new QTableWidgetItem(QString(list[1])+QString("  正常值(%1~%2)").arg(QString::number(m_userConfig->csqMin)).arg(QString::number(m_userConfig->csqMax)));
                        ui->tableWidget_operate->setItem(RESULT_CSQ_LINE,1,csqShowItem);
                    }
                    pushButton_resultShow(ui->tableWidget_operate->cellWidget(RESULT_CSQ_LINE,2), true);

                }
                else if(1 == internetResult)
                {
                    setPushButtonStatus(3);
                    m_checkDeviceInfo->csqResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_ERROR);
                    m_userInfo.currentEventLine = RESULT_CSQ_LINE;
                    if(NULL != ui->tableWidget_operate->item(RESULT_CSQ_LINE,1))
                    {
                        ui->tableWidget_operate->item(RESULT_CSQ_LINE,1)->setText(QString(list[1])+QString("  正常值(%1~%2)").arg(QString::number(m_userConfig->csqMin)).arg(QString::number(m_userConfig->csqMax)));
                    }
                    else
                    {
                        QTableWidgetItem *csqShowItem = new QTableWidgetItem(QString(list[1])+QString("  正常值(%1~%2)").arg(QString::number(m_userConfig->csqMin)).arg(QString::number(m_userConfig->csqMax)));
                        ui->tableWidget_operate->setItem(RESULT_CSQ_LINE,1,csqShowItem);
                    }
                    qDebug()<<"here11111";
                    pushButton_resultShow(ui->tableWidget_operate->cellWidget(RESULT_CSQ_LINE,2),false);

                }
                else
                {
                    m_checkDeviceInfo->csqResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_ERROR);
                    setPushButtonStatus(2);
                    setPushButtonStatus(2,m_userInfo.currentEventLine+1);
                    pushButton_resultShow(ui->tableWidget_operate->cellWidget(RESULT_INTERNET_LINE,2),false);
                }
            }
            else
            {
                serialAckTimer->start();
//                pushButton_resultShow(ui->tableWidget_operate->cellWidget(widgetNum,2),0);
            }
            break;
        case RESULT_CSQ_LINE:
            logTextEditDisplay(expendShowData(QString("接收到%1指令反馈").arg(RESULT_MESS_STRING(widgetNum)),RECV_TYPR));
            if(2 == list.size())
            {
                int csq = QString(list[1]).toInt();
                m_checkDeviceInfo->csq = csq;
                bool csqIsVaild = true;
                if((csq > m_userConfig->csqMin && csq < m_userConfig->csqMax))
                {
                    csqIsVaild = true;
                }
                else
                {
                    csqIsVaild = false;
                }
                if(NULL != ui->tableWidget_operate->item(RESULT_CSQ_LINE,1))
                {
                    ui->tableWidget_operate->item(RESULT_CSQ_LINE,1)->setText(QString(list[1])+QString("  正常值(%1~%2)").arg(QString::number(m_userConfig->csqMin)).arg(QString::number(m_userConfig->csqMax)));
                }
                else
                {
                    QTableWidgetItem *csqShowItem = new QTableWidgetItem(QString(list[1])+QString("  正常值(%1~%2)").arg(QString::number(m_userConfig->csqMin)).arg(QString::number(m_userConfig->csqMax)));
                    ui->tableWidget_operate->setItem(RESULT_CSQ_LINE,1,csqShowItem);
                }
                pushButton_resultShow(ui->tableWidget_operate->cellWidget(RESULT_CSQ_LINE,2),csqIsVaild);
            }
            else
            {
                serialAckTimer->start();
            }
            break;
        default:
            qCritical()<<"无法识别";
            break;

    }
}

void MainWindow::uiAttributeSet()
{

    /* 表格设置 */  // 缺乏对参数项的放大
    ui->tableWidget_operate->verticalHeader()->setHidden(true);
    ui->tableWidget_operate->setRowCount(7);
    ui->tableWidget_operate->setShowGrid(false);
    QStringList nameList;
    nameList<<"模组信息"<<"指示灯"<<"蜂鸣器"<<"按键"<<"干簧管"<<"联网"<<"信号值";
    setTableWidgetItems(0,ui->tableWidget_operate,0,0,nameList);
    QString qssTV = "QHeaderView::section,QTableCornerButton:section{ \
               padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424; \
       border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
   background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
   "QTableWidget{background-color:white;border:none;}";
//    ui->tableWidget_operate->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget_operate->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget_operate->setStyleSheet(qssTV);
    ui->tableWidget_operate->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->tableWidget_operate->horizontalHeader()->setEnabled(false);
    // 暂不做对按钮的press动作进行限制

    for(int i =0;i<ui->tableWidget_operate->rowCount();i++)
    {
        QPushButton *pushButton = new QPushButton("未检测");
        pushButton->setFlat(true);
        pushButton->setObjectName(QString("pushButton_%1").arg(i));
        connect(pushButton,SIGNAL(clicked()),this,SLOT(pushButtonOperate()));
        ui->tableWidget_operate->setCellWidget(i,2,pushButton);
        ui->tableWidget_operate->item(i,0)->setTextAlignment(Qt::AlignCenter);
    }



    /*  */
    ui->checkBox_serialShow->setLayoutDirection(Qt::RightToLeft);

    ui->textEdit_serialRecvDataShow->document()->setMaximumBlockCount(1000);

    ui->radioButton_ready->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    ui->radioButton_ready->setStyleSheet("QRadioButton{font-size:24px} QRadioButton::indicator::checked {border-radius:7px;background-color:green;border:2px solid white;};");
    ui->tableWidget_operate->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    ui->commandLinkButton_checkHistory->setIcon(QIcon(":/image/historyShow.png"));
    ui->commandLinkButton_config->setIcon(QIcon(":/image/deviceConfig.png"));

    ui->label_barCodeShow2->setVisible(false);
    ui->pushButton_barCodeFold2->setText("展开");

    ui->tableWidget_operate->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QFont font("Microsoft YaHei",100); //font: 24pt "宋体";
    font.setLetterSpacing(QFont::PercentageSpacing,50); //设置字间距 默认100%
    font.setLetterSpacing(QFont::AbsoluteSpacing,5); //默认1
    ui->pushButton_check->setFont(font);


    // 菜单栏检测
    connect(ui->menu,SIGNAL(triggered(QAction*)),this,SLOT(action_handle(QAction*)));
//    ui->action_updateVersion->setVisible(false);
    ui->menu->menuAction()->setVisible(false);
}

void MainWindow::initInfo()
{
    m_userConfig = new userConfig();
    memset(&m_userInfo,0,sizeof(m_userInfo));
    m_userInfo.currentEventLine = RESULT_CHECK_INIT;
    // 用户信息界面
    m_userInfoFunc = new userInfo();
    connect(m_userInfoFunc,SIGNAL(signalUserInfo(QStringList)),this,SLOT(recvUserInfo(QStringList)));
    m_userInfoFunc->show();


    serialAckTimer = new QTimer();
    connect(serialAckTimer,SIGNAL(timeout()),this,SLOT(waitAckTimeout()));

    setCheckEnableStatus(false);

    m_checkDeviceInfo = new checkStatusInfo();

    showQrcodeList.append(1);
    showQrcodeList.append(3);

#ifdef USE_UPDATE_INLINE
    updateVersion = new toolUpdate();
    ((toolUpdate *)updateVersion)->checkForUpdates();
#endif
}

void MainWindow::setCheckEnableStatus(bool status)
{
    if(status)
    {
        ui->pushButton_check->setEnabled(true);
        ui->pushButton_check->setStyleSheet("QPushButton{font: 18pt \"宋体\";background-color:rgb(0, 170, 255);}");
        ui->radioButton_ready->setChecked(true);
        m_userConfig->deviceReady = true;
    }
    else
    {
        ui->pushButton_check->setEnabled(false);
        ui->pushButton_check->setStyleSheet("QPushButton{font: 18pt \"宋体\";background-color:rgb(225, 225, 225);}");
        ui->radioButton_ready->setChecked(false);
        m_userConfig->deviceReady = false;
        // 清空当前的正在执行的条件信息
        isChecking = false;
    }
}


/**************************************************************************
** 功能	@brief : 设置tablewidget的数据
** 输入	@param : sortMode 0:列 1：行
**               widget: QTableWidget表格示例
**               startPlace: 行/列 起始位置
**               otherPlace: 行/列 固定位置
**               dataList:   内容
** 输出	@retval:
***************************************************************************/
void MainWindow::setTableWidgetItems(int sortMode, QWidget *widget,int startPlace,int otherPlace,QStringList dataList)
{
    if(0 == sortMode)
    {
        if(((QTableWidget *)widget)->rowCount()-startPlace < dataList.size())
        {
            return;
        }
        for(int i =0;i<dataList.size();i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(dataList.at(i));
            ((QTableWidget *)widget)->setItem(startPlace+i,otherPlace,item);
        }
    }
    else
    {
        if(((QTableWidget *)widget)->columnCount()-startPlace < dataList.size())
        {
            return;
        }
        for(int i =0;i<dataList.size();i++)
        {
            QTableWidgetItem *item = new QTableWidgetItem(dataList.at(i));
            ((QTableWidget *)widget)->setItem(otherPlace,startPlace+i,item);
        }
    }
}

void MainWindow::setPushButtonFont(QWidget *widget, int mode)
{
    if(1 == mode)
    {
        widget->setStyleSheet("font: 10pt \"宋体\";color:rgb(255,165,0);text-decoration:underline;");
    }
    else if(0 == mode)
    {
        widget->setStyleSheet("font: 10pt \"宋体\";color:rgb(0,0,0);");
    }
}

void MainWindow::pushButtonOperate()
{
    if(serialAckTimer->isActive())
    {
        serialAckTimer->stop();
    }
    QPushButton *sigBtn = qobject_cast<QPushButton*>(sender());
    QString value =sigBtn->objectName().split('_').last();
    qInfo()<<"pushButtonOperate line:"<<value;
    m_userInfo.currentEventLine = value.toInt();
    if(m_userInfo.currentEventLine >= RESULT_MODULE_INFO_LINE)
    {
        QWidget *widget = ui->tableWidget_operate->cellWidget(m_userInfo.currentEventLine,2);
        if(((QPushButton *)widget)->text().isEmpty())
        {
            return;
        }
    }

    qDebug()<<"sigBtn->text():"<<sigBtn->text();
    if(0 == sigBtn->text().compare("未检测"))
    {
        setPushButtonStatus(1);
    }
    else
    {
//        qDebug()<<"m_userInfo.currentEventLine:"<<m_userInfo.currentEventLine;
//        for(int i = m_userInfo.currentEventLine;i<RESULT_CSQ_LINE;i++)
//        {
//            qDebug()<<"i-1:"<<i-1;
//            QString currentResult = ((QPushButton *)(ui->tableWidget_operate->cellWidget(i-1,2)))->toolTip();
//            if((0 == currentResult.compare("成功")) || (0 == currentResult.compare("失败")))
//            {
//                m_userInfo.currentEventLine++;
//            }
//        }
//        if(RESULT_CSQ_LINE == m_userInfo.currentEventLine)
//        {
//            operateNextEvent();
//            return;
//        }
    }
    // 发送指令
    if(sendProtocolData(m_userInfo.currentEventLine))
    {
        logTextEditDisplay(expendShowData(QString("已发送%1信息指令").arg(RESULT_MESS_STRING(m_userInfo.currentEventLine))));
    }
    if(NULL !=currentDialog)
    {
        currentDialog->close();
        currentDialog->deleteLater();
        currentDialog = NULL;
    }
    // 显示操作窗口
    if(m_userInfo.currentEventLine >= RESULT_LED_LINE && m_userInfo.currentEventLine <= RESULT_BEER_LINE)
    {
        DialogSetResult *setResultDialog = new DialogSetResult(QString("%1是否正常工作？").arg(RESULT_MESS_STRING(m_userInfo.currentEventLine)));
        connect(setResultDialog,SIGNAL(signalResult(int,QStringList)),this,SLOT(recvResultDialog(int,QStringList)));
//        setResultDialog->setGeometry(QWidget::mapToGlobal(this->pos()).x()+this->width()/4,QWidget::mapToGlobal(this->pos()).y()+this->height()/3,this->width()/2,this->height()/3);
        QRect rect = this->geometry();
        setResultDialog->move(rect.x()+rect.width()/2-setResultDialog->width()/2,rect.y()+rect.height()/2-setResultDialog->height()/2);
        currentDialog = setResultDialog;
        m_userConfig->isNeedOperate = true;
        setResultDialog->show();
    }
    else if(m_userInfo.currentEventLine >= RESULT_PUSHBUTTON_LINE && m_userInfo.currentEventLine <= RESULT_REED_LINE)
    {

        DialogTipsCombinedEnd *tipsWidget;
        if(RESULT_PUSHBUTTON_LINE == m_userInfo.currentEventLine)
        {
            tipsWidget= new DialogTipsCombinedEnd(QString("当前正在检测\"%1\"，请<font color=#f59a23>按下</font>\"%1\"！").arg(RESULT_MESS_STRING(m_userInfo.currentEventLine)));
            m_checkDeviceInfo->pushButtonResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_ERROR);
        }
        else
        {
            tipsWidget= new DialogTipsCombinedEnd(QString("当前正在检测\"%1\"，请将设备<font color=#f59a23>靠近</font>磁铁1.5cm的位置！").arg(RESULT_MESS_STRING(m_userInfo.currentEventLine)));
            m_checkDeviceInfo->reedResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_ERROR);
        }
        connect(tipsWidget,SIGNAL(rejected()),this,SLOT(recvErrorDialogSelect()));
//        tipsWidget->setGeometry(QWidget::mapToGlobal(this->pos()).x()+this->width()/4,QWidget::mapToGlobal(this->pos()).y()+this->height()/3,this->width()/2,this->height()/3);
        QRect rect = this->geometry();
        tipsWidget->move(rect.x()+rect.width()/2-tipsWidget->width()/2,rect.y()+rect.height()/2-tipsWidget->height()/2);
        currentDialog = tipsWidget;
        tipsWidget->show();
        QTimer *showTimer = new QTimer();
        showTimer->singleShot(3000, tipsWidget, [=](){tipsWidget->showPushButton();});
    }
    else
    {

    }
    // 这里莫名运行三次，后续跟进
    if(RESULT_LED_LINE == m_userInfo.currentEventLine || RESULT_BEER_LINE == m_userInfo.currentEventLine)
    {
        return;
    }
    else if(RESULT_PUSHBUTTON_LINE == m_userInfo.currentEventLine || RESULT_REED_LINE == m_userInfo.currentEventLine )
    {
        serialAckTimer->setInterval(30*1000);
    }
    else
    {
        serialAckTimer->setInterval(10*1000);
    }
    serialAckTimer->start();

}


void MainWindow::recvErrorDialogSelect(int mode)
{
    m_userConfig->isNeedOperate = false;
    if(NULL != serialAckTimer && serialAckTimer->isActive())
    {
        serialAckTimer->stop();
    }
    switch(mode)
    {
        case SIGNAL_STOP_CHECK:
        {
            // 弹窗是否保存记录
            if(m_userInfo.currentEventLine < RESULT_CSQ_LINE)
            {
                m_userInfo.checkDeviceResult = false;
                setPushButtonStatus(2+m_userInfo.checkDeviceResult);
            }
            // 发送结束检测的指令
            sendSerialCheckEnd();
//            saveMessageBoxShow();
            break;
        }
        case SIGNAL_RETRY:
        {
            // 重置选项信息并发送
            setPushButtonStatus(0);
            if(m_userConfig->deviceReady)
            {
                QWidget *pushButtonWidget = ui->tableWidget_operate->cellWidget(m_userInfo.currentEventLine,2);
                qDebug()<<__LINE__<<"SIGNAL_RETRY click";
                ((QPushButton *)pushButtonWidget)->click();
            }
            else
            {
                QMessageBox::critical(this,"错误","当前串口或打印机异常","关闭");
            }
            break;
        }
        case SIGNAL_NEXT:
        {
            // 标识更新
            m_userInfo.checkDeviceResult = false;
            setPushButtonStatus(2+m_userInfo.checkDeviceResult);
            operateNextEvent();
            break;
        }
        default:
            break;
    }
}

void MainWindow::recvErrorDialogSelect()
{
    qDebug()<<"recvErrorDialogSelect";
    m_userConfig->isNeedOperate = false;
    if(NULL != currentDialog && currentDialog->isActiveWindow())
    {
        disconnect(currentDialog,SIGNAL(rejected()),this,SLOT(recvErrorDialogSelect()));
        currentDialog->close();
        currentDialog = NULL;
    }
    if(NULL != serialAckTimer && serialAckTimer->isActive())
    {
        serialAckTimer->stop();
    }
    switch(errorSelectNum)
    {
        case SIGNAL_STOP_CHECK:
        {
            m_userInfo.checkDeviceResult = false;
            if(m_userInfo.currentEventLine < RESULT_CSQ_LINE)
            {
                setPushButtonStatus(2+m_userInfo.checkDeviceResult);
            }
            // 发送结束检测的指令
            sendSerialCheckEnd();
//            // 弹窗是否保存记录
//            saveMessageBoxShow();
            break;
        }
        case SIGNAL_RETRY:
        {
            // 重置选项信息并发送
            setPushButtonStatus(0);
            if(m_userConfig->deviceReady)
            {
                QWidget *pushButtonWidget = ui->tableWidget_operate->cellWidget(m_userInfo.currentEventLine,2);
                qDebug()<<__LINE__<<"SIGNAL_RETRY click";
                ((QPushButton *)pushButtonWidget)->click();
            }
            else
            {
                QMessageBox::critical(this,"错误","当前串口或打印机异常","关闭");
            }
            break;
        }
        case SIGNAL_NEXT:
//        case SIGNAL_INIT:
        {
            // 标识更新
            m_userInfo.checkDeviceResult = false;
            setPushButtonStatus(2+m_userInfo.checkDeviceResult);

            operateNextEvent();
            break;
        }
        case SIGNAL_FAILED:
        {
            // 标识更新
            m_userInfo.checkDeviceResult = false;
            setPushButtonStatus(2+m_userInfo.checkDeviceResult);
            operateNextEvent();
            break;
        }
        default:
            break;
    }
}

void MainWindow::waitAckTimeout()
{
    // 哪个事件的timeout
    qDebug()<<"end timer:"<<serialAckTimer->interval()<< "m_userInfo.currentEventLine"<<m_userInfo.currentEventLine;
    qDebug()<<"m_userConfig->isCheckingInit:"<<m_userConfig->isCheckingInit<<"m_userConfig->isCheckingEnd:"<<m_userConfig->isCheckingEnd;
    serialAckTimer->stop();
    // 补丁-init
    if(m_userConfig->isCheckingInit)
    {
        m_userConfig->isCheckingInit = false;
        QMessageBox::critical(this,"错误","等待产测初始化反馈指令超时，请重新点击“检测”按钮进行检测或更换设备","确认");
        logTextEditDisplay(expendShowData("等待产测初始化反馈指令超时，请重新点击“检测”按钮进行检测或更换设备"));
        isChecking = false;
    }
    else if(m_userConfig->isCheckingEnd)
    {
        m_userConfig->isCheckingEnd = false;
        logTextEditDisplay(expendShowData("等待结束检测反馈指令超时"));
        saveMessageBoxShow();
    }
    else
    {
        pushButton_resultShow(ui->tableWidget_operate->cellWidget(m_userInfo.currentEventLine,2),0);
    }


}

void MainWindow::recvResultDialog(int mode, QStringList list)
{

    m_userConfig->isNeedOperate = false;
    if(NULL != currentDialog && currentDialog->isActiveWindow())
    {
        disconnect(currentDialog,SIGNAL(signalResult(int,QStringList)),this,SLOT(recvResultDialog(int,QStringList)));
        currentDialog->close();
        currentDialog = NULL;
    }
    switch(mode)
    {
        case SIGNAL_STOP_CHECK:
            // 弹窗是否保存记录
            if(list.size() >=1)
            {
                if(RESULT_LED_LINE == m_userInfo.currentEventLine)
                {
                    qDebug()<<"m_checkDeviceInfo->ledResult:"<<m_checkDeviceInfo->ledResult;
                    m_checkDeviceInfo->ledResult = RESULT_STATUS_STRING((1==QString(list[0]).toInt()?DEV_FUNC_STATUS_OK:DEV_FUNC_STATUS_ERROR));
                }
                else if(RESULT_BEER_LINE == m_userInfo.currentEventLine)
                {
                    m_checkDeviceInfo->beerResult = RESULT_STATUS_STRING((1==QString(list[0]).toInt()?DEV_FUNC_STATUS_OK:DEV_FUNC_STATUS_ERROR));
                }
                setPushButtonStatus(2+QString(list[0]).toInt());
            }
            else
            {
                setPushButtonStatus(2);
            }
            if(m_userInfo.currentEventLine < RESULT_CSQ_LINE)
            {
                m_userInfo.checkDeviceResult = false;
            }
            // 发送结束检测的指令
            sendSerialCheckEnd();
//            saveMessageBoxShow();
            break;
        case SIGNAL_RETRY:
            // 重置选项信息并发送
            setPushButtonStatus(0);
            if(m_userConfig->deviceReady)
            {
                QWidget *pushButtonWidget = ui->tableWidget_operate->cellWidget(m_userInfo.currentEventLine,2);
                qDebug()<<__LINE__<<"SIGNAL_RETRY click";
                ((QPushButton *)pushButtonWidget)->click();
            }
            else
            {
                QMessageBox::critical(this,"错误","当前串口或打印机异常","关闭");
            }
            break;
        case SIGNAL_NEXT:
            // 标识更新
            if(list.size() >=1)
            {
                qDebug()<<"m_userInfo.currentEventLine:"<<m_userInfo.currentEventLine<<",list:"<<list<<"QString(list[0]).toInt():"<<QString(list[0]).toInt();
                if(RESULT_LED_LINE == m_userInfo.currentEventLine)
                {
                    m_checkDeviceInfo->ledResult = RESULT_STATUS_STRING((1==QString(list[0]).toInt()?DEV_FUNC_STATUS_OK:DEV_FUNC_STATUS_ERROR));
                    qDebug()<<"m_checkDeviceInfo->ledResult:"<<m_checkDeviceInfo->ledResult;
                }
                else if(RESULT_BEER_LINE == m_userInfo.currentEventLine)
                {
                    m_checkDeviceInfo->beerResult = RESULT_STATUS_STRING((1==QString(list[0]).toInt()?DEV_FUNC_STATUS_OK:DEV_FUNC_STATUS_ERROR));
                }
                if(0 == QString(list[0]).toInt())
                {
                    m_userInfo.checkDeviceResult = false;
                }

                setPushButtonStatus(2+QString(list[0]).toInt());
            }
            operateNextEvent();
            break;
        default:
            break;
    }
}

void MainWindow::GenerateQRcode(QString tempstr, QLabel *label)
{
    QRcode *qrcode; //二维码数据
    //QR_ECLEVEL_Q 容错等级
    qrcode = QRcode_encodeString(tempstr.toStdString().c_str(), 2, QR_ECLEVEL_Q, QR_MODE_8, 1);
    qint32 temp_width=200; //二维码图片的大小
    qint32 temp_height=200;
    label->setFixedSize(temp_width,temp_height);

    qint32 qrcode_width = qrcode->width > 0 ? qrcode->width : 1;
    double scale_x = (double)temp_width / (double)qrcode_width; //二维码图片的缩放比例
    double scale_y =(double) temp_height /(double) qrcode_width;
    QImage mainimg=QImage(temp_width,temp_height,QImage::Format_ARGB32);
    QPainter painter(&mainimg);
    QColor background(Qt::white);
    painter.setBrush(background);
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, 0, temp_width, temp_height);
    QColor foreground(Qt::black);
    painter.setBrush(foreground);
    for( qint32 y = 0; y < qrcode_width; y ++)
    {
        for(qint32 x = 0; x < qrcode_width; x++)
        {
            unsigned char b = qrcode->data[y * qrcode_width + x];
            if(b & 0x01)
            {
                QRectF r(x * scale_x, y * scale_y, scale_x, scale_y);
                painter.drawRects(&r, 1);
            }
        }
    }
    QPixmap mainmap=QPixmap::fromImage(mainimg);
    label->setPixmap(mainmap);
    label->setVisible(true);
}


void MainWindow::pushButton_resultShow(QWidget *widget, int result)
{
    ((QPushButton*)widget)->setText("");
    // 待植入正确或错误的ICON
    if(0 == result)
    {
        setPushButtonStatus(2);
        // 错误弹窗
        DialogOperateErrorCreate();
    }
    else if(1 == result) // 成功
    {
        setPushButtonStatus(3);
        // 下一个指令发送
        operateNextEvent();
    }
}

void MainWindow::operateNextEvent()
{
    qDebug()<<"deviceReady:"<<m_userConfig->deviceReady<<"currentEventLine:"<<m_userInfo.currentEventLine;
    if(m_userInfo.currentEventLine >= RESULT_CSQ_LINE || (m_userInfo.currentEventLine == RESULT_INTERNET_LINE && NULL != ui->tableWidget_operate->cellWidget(RESULT_CSQ_LINE,2) && (!ui->tableWidget_operate->cellWidget(RESULT_CSQ_LINE,2)->toolTip().isEmpty()) ))
    {
        // 发送结束检测的指令
        sendSerialCheckEnd();
    }
    else if(m_userInfo.currentEventLine < RESULT_INTERNET_LINE)
    {
        m_userInfo.currentEventLine++;
        if(m_userConfig->deviceReady)
        {
            if(m_userInfo.currentEventLine == RESULT_MODULE_INFO_LINE && false == m_userInfo.checkDeviceResult)
            {
                QMessageBox *msgBox = new QMessageBox(QMessageBox::Critical, "错误","当前检测必须存在相应的设备IMEI和版本号，即将关闭本次测试", QMessageBox::Yes);
                msgBox->button(QMessageBox::Yes)->setText("确认");
                QTimer::singleShot(3000,msgBox,SLOT(accept()));
                msgBox->exec();
            }
            else
            {
                QWidget *pushButtonWidget = ui->tableWidget_operate->cellWidget(m_userInfo.currentEventLine,2);
                ((QPushButton *)pushButtonWidget)->click();
            }
        }
        else
        {
            QMessageBox::critical(this,"错误","当前串口或打印机异常","关闭");
        }
    }
}

void MainWindow::saveMessageBoxShow()
{
    qDebug()<<__FUNCTION__;
    if(RESULT_CHECK_INIT == m_userInfo.currentEventLine)
    {
        resetCheck();
        return;
    }
    bool isSaveHistory = true;
    QString showText;
    if(m_userInfo.checkDeviceResult && m_userInfo.currentEventLine >= RESULT_CSQ_LINE)
    {
        showText = "通过";
    }
    else
    {
        showText = "不通过";
    }
    errorSelectNum = SIGNAL_INIT;
    DialogTotalResult *dialogResult = new DialogTotalResult(showText);
    QRect rect = this->geometry();
    dialogResult->move(rect.x()+rect.width()/2-dialogResult->width()/2,rect.y()+rect.height()/2-dialogResult->height()/2);
    dialogResult->exec();
    if(SIGNAL_OK == errorSelectNum)
    {
    }
    else if (SIGNAL_FAILED == errorSelectNum)
    {
        m_userInfo.checkDeviceResult = false;
    }
    else
    {
        isSaveHistory = false;
        m_userInfo.checkDeviceResult = false;
    }

    qInfo()<<"保存记录";
    if(NULL != m_saveRecordFunc && RESULT_MODULE_INFO_LINE != m_userInfo.currentEventLine && isSaveHistory)
    {
        if(!((saveRecord *)m_saveRecordFunc)->isOpenTable())
        {
            ((saveRecord *)m_saveRecordFunc)->openSqlDataBase(m_userConfig->batch);
        }
        QStringList list;
        if(NULL == m_userConfig)
        {
            qCritical()<<"m_userConfig is empty";
        }
        // 这里待优化
        m_checkDeviceInfo->pushButtonResult = getTableWidgetItemResult(RESULT_PUSHBUTTON_LINE);
        m_checkDeviceInfo->reedResult = getTableWidgetItemResult(RESULT_REED_LINE);

        list<<m_checkDeviceInfo->IMEI<<m_userConfig->moduleType<<m_userConfig->batch<<m_checkDeviceInfo->sn<<m_userConfig->personName<<(QDateTime::currentDateTime()).toString();
        list<<m_checkDeviceInfo->version<<m_checkDeviceInfo->ledResult<<m_checkDeviceInfo->beerResult<<m_checkDeviceInfo->pushButtonResult<<m_checkDeviceInfo->reedResult;
        list<<m_checkDeviceInfo->internelResult<<QString::number(m_checkDeviceInfo->csq)<<m_checkDeviceInfo->getTotalResult();

        qDebug()<<"list:"<<list;
        if(!((saveRecord *)m_saveRecordFunc)->insertHistoryToSql(list))
        {
            QMessageBox *msgBox = new QMessageBox(QMessageBox::Warning, "警告", QString("历史存在此IMEI:%1,是否覆盖").arg(list[0]), QMessageBox::Yes | QMessageBox::No);
            msgBox->setDefaultButton(QMessageBox::Yes);
            msgBox->button(QMessageBox::Yes)->setText("确认");
            msgBox->button(QMessageBox::No)->setText("取消");
            int ret = msgBox->exec();
            if(QMessageBox::Yes == ret)
            {
                ((saveRecord *)m_saveRecordFunc)->modifyHistoryToSql(list);
            }
            else
            {
                qDebug()<< "ret:"<<ret;
            }
        }

    }
    if(m_userInfo.checkDeviceResult)
    {
        QMessageBox *msgBox = new QMessageBox(QMessageBox::NoIcon, "打印二维码", "二维码打印中...");
        QTimer::singleShot(3000,msgBox,SLOT(accept()));
        msgBox->exec();
        if(m_userConfig->deviceReady)
        {
            // 需确保有此信息
            QPixmap pixmap =  ui->label_barCodeShow3->grab();
            ((communiteConfigure *)m_comuniteConfigureFunc)->printerPrintImage(&pixmap);
        }

    }
    resetCheck();
//    if(RESULT_MODULE_INFO_LINE == m_userInfo.currentEventLine)
//    {
//        resetPushButtonStatus();
//        resetLaberShowBarcode();
//    }
//    isChecking = false;
}

void MainWindow::setPushButtonStatus(int mode,int rowLine)
{
    int operateLine = -1;
    if(-1 == rowLine)
    {
        operateLine = m_userInfo.currentEventLine;
    }
    else
    {
        operateLine = rowLine;
    }
    QWidget *widget = ui->tableWidget_operate->cellWidget(operateLine,2);
    if(NULL == widget)
    {
        return;
    }
    qDebug()<<"mode:"<<mode<<"operateLine:"<<operateLine;
    switch (mode)
    {
        case 0:
            setPushButtonFont(widget,0);
            ((QPushButton *)widget)->setText("未检测");
            ((QPushButton *)widget)->setToolTip("未检测");
            ((QPushButton *)widget)->setIcon(QIcon());
            break;
        case 1:
            setPushButtonFont(widget,1);
            ((QPushButton *)widget)->setText("检测中");
            ((QPushButton *)widget)->setToolTip("检测中");
            // 需要加入等待中的图片 ，第二版改为动图
            ((QPushButton *)widget)->setIcon(QIcon());
            ((QPushButton *)widget)->setLayoutDirection(Qt::RightToLeft);
            break;
        case 2: // 错误
            ((QPushButton *)widget)->setText("");
            ((QPushButton *)widget)->setToolTip("失败");
            // 需要加入等待中的图片
            ((QPushButton *)widget)->setIcon(QIcon(":/image/check_func_failed.png"));
            break;
        case 3: // 正确
            ((QPushButton *)widget)->setText("");
            ((QPushButton *)widget)->setToolTip("成功");
            // 需要加入等待中的图片
            ((QPushButton *)widget)->setIcon(QIcon(":/image/check_func_ok.png"));

            break;
        default:
            break;
    }
    ((QPushButton *)widget)->repaint();
    ((QPushButton *)widget)->update();
    ui->tableWidget_operate->viewport()->update();
}

void MainWindow::resetPushButtonStatus()
{
    for(int i = 0;i<ui->tableWidget_operate->rowCount();i++)
    {
        QWidget *widget = ui->tableWidget_operate->cellWidget(i,2);
        qDebug()<<"className:"<<widget->metaObject()->className();
        if(NULL != widget && QString(widget->metaObject()->className()).contains("QPushButton"))
        {
            setPushButtonStatus(0,i);
        }
    }
    if(NULL != ui->tableWidget_operate->item(0,1))
    {
        ui->tableWidget_operate->item(0,1)->setText("");
    }
    //    isChecking = false;
}

void MainWindow::resetLaberShowBarcode()
{
    ui->label_barCodeShow->clear();
    ui->label_barCodeShow2->clear();
    ui->label_barCodeShow3->clear();
}

void MainWindow::resetCheck()
{
    resetPushButtonStatus();
    resetLaberShowBarcode();
    m_userInfo.currentEventLine = RESULT_CHECK_INIT;
    isChecking = false;
    m_userConfig->isCheckingEnd = false;
    m_userConfig->isCheckingInit = false;
    m_userConfig->isNeedOperate = false;
    if(NULL != ui->tableWidget_operate->item(RESULT_CSQ_LINE,1))
    {
        ui->tableWidget_operate->item(RESULT_CSQ_LINE,1)->setText("");
    }
    if(NULL != currentDialog && currentDialog->isActiveWindow())
    {
        currentDialog->close();
        currentDialog = NULL;
    }
    if(NULL != serialAckTimer && serialAckTimer->isActive())
    {
        serialAckTimer->stop();
    }
    m_checkDeviceInfo->init();

}

void MainWindow::communiteConfigWidgetShow()
{
    // 用户配置界面
    if(NULL == m_comuniteConfigureFunc)
    {
        m_comuniteConfigureFunc = new communiteConfigure();
        connect(m_comuniteConfigureFunc,SIGNAL(openDeviceSuccess(QStringList)),this,SLOT(recvOpenDeviceSuccess(QStringList)));
        connect(m_comuniteConfigureFunc,SIGNAL(signalDeviceStatus(QStringList)),this,SLOT(recvDeviceErrorStatus(QStringList)));
        connect(m_comuniteConfigureFunc,SIGNAL(signalWidgetChange(int,QStringList)),this,SLOT(recvWidgetInfoChange(int,QStringList)));
        connect(m_comuniteConfigureFunc,SIGNAL(signalSerialRecvData(QByteArray)),this,SLOT(recvSerialRecvData(QByteArray)));  // ,Qt::QueuedConnection
//        m_comuniteConfigureFunc->setWindowFlags(m_comuniteConfigureFunc->windowFlags() |  Qt::WindowStaysOnTopHint);
        ((communiteConfigure *)m_comuniteConfigureFunc)->widgetShow(this->geometry());
    }
    else
    {
        ((communiteConfigure *)m_comuniteConfigureFunc)->widgetShow(this->geometry());
    }
}

bool MainWindow::sendProtocolData(int QPushButtonNum)
{
    bool ret = false;
    if(NULL != m_comuniteConfigureFunc)
    {
        switch(QPushButtonNum)
        {
            case RESULT_MODULE_INFO_LINE:
                ret=((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolBasicData(1,CMD_MODULE_INFO_GET);
                break;
            case RESULT_LED_LINE:
                ret=((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolBasicData(1,CMD_LED_CHECK);
                break;
            case RESULT_BEER_LINE:
                ret=((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolBasicData(1,CMD_BEER_CHECK);
                break;
            case RESULT_PUSHBUTTON_LINE:
            {
                ret=((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolBasicData(1,CMD_PUSHBUTTON_CHECK);
                break;
            }
            case RESULT_REED_LINE:
                ret=((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolBasicData(1,CMD_REED_CHECK);
                break;
            case RESULT_INTERNET_LINE:
            case RESULT_CSQ_LINE:
            {
                ret=((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolBasicData(1,CMD_INTERNET_CHECK);
                break;
            }
            default:
                break;
        }
    }
    return ret;
}

void MainWindow::DialogOperateErrorCreate()
{
    DialogOperateError *errorDialog = NULL;
    qDebug()<<__FUNCTION__<<"m_userInfo.currentEventLine:"<<m_userInfo.currentEventLine;
    switch(m_userInfo.currentEventLine)
    {
        case RESULT_MODULE_INFO_LINE:
//        case RESULT_LED_LINE:
//        case RESULT_BEER_LINE:
        case RESULT_PUSHBUTTON_LINE:
        case RESULT_REED_LINE:
        case RESULT_INTERNET_LINE:
        case RESULT_CSQ_LINE:
        {
            errorDialog = new DialogOperateError(RESULT_MESS_STRING(m_userInfo.currentEventLine));
            if(RESULT_MODULE_INFO_LINE == m_userInfo.currentEventLine)
            {
                errorDialog->setNoNext();
            }
            break;
        }
        default:
            break;
    }
    if(NULL == errorDialog)
    {
        return;
    }
    connect(errorDialog,SIGNAL(rejected()),this,SLOT(recvErrorDialogSelect()));
//    errorDialog->setGeometry(QWidget::mapToGlobal(this->pos()).x()+this->width()/4,QWidget::mapToGlobal(this->pos()).y()+this->height()/3,this->width()/2,this->height()/3);
    QRect rect = this->geometry();
    errorDialog->move(rect.x()+rect.width()/2-errorDialog->width()/2,rect.y()+rect.height()/2-errorDialog->height()/2);

    if(NULL !=currentDialog)
    {
        currentDialog->close();
        currentDialog->deleteLater();
        currentDialog = NULL;
    }
    currentDialog = errorDialog;
    errorDialog->show();
    m_userConfig->isNeedOperate = true;
}

void MainWindow::changeQrcodeWidget(int changeWidget, bool flag)
{
    switch(changeWidget)
    {
        case 1:
            if(true == flag)
            {
                if(!ui->label_barCodeShow->isVisible())
                {
                    ui->label_barCodeShow->setVisible(true);
                }
                if(0 == ui->pushButton_barCodeFold->text().compare("展开"))
                {
                    ui->pushButton_barCodeFold->setText("收起");
                }
            }
            else
            {
                if(ui->label_barCodeShow->isVisible())
                {
                    ui->label_barCodeShow->setVisible(false);
                }
                if(0 == ui->pushButton_barCodeFold->text().compare("收起"))
                {
                    ui->pushButton_barCodeFold->setText("展开");
                }
            }
            break;
        case 2:
            if(true == flag)
            {
                if(!ui->label_barCodeShow2->isVisible())
                {
                    ui->label_barCodeShow2->setVisible(true);
                }
                if(0 == ui->pushButton_barCodeFold2->text().compare("展开"))
                {
                    ui->pushButton_barCodeFold2->setText("收起");
                }
            }
            else
            {
                if(ui->label_barCodeShow2->isVisible())
                {
                    ui->label_barCodeShow2->setVisible(false);
                }
                if(0 == ui->pushButton_barCodeFold2->text().compare("收起"))
                {
                    ui->pushButton_barCodeFold2->setText("展开");
                }
            }
            break;
        case 3:
            if(true == flag)
            {
                if(!ui->label_barCodeShow3->isVisible())
                {
                    ui->label_barCodeShow3->setVisible(true);
                }
                if(0 == ui->pushButton_barCodeFold3->text().compare("展开"))
                {
                    ui->pushButton_barCodeFold3->setText("收起");
                }
            }
            else
            {
                if(ui->label_barCodeShow3->isVisible())
                {
                    ui->label_barCodeShow3->setVisible(false);
                }
                if(0 == ui->pushButton_barCodeFold3->text().compare("收起"))
                {
                    ui->pushButton_barCodeFold3->setText("展开");
                }
            }
            break;
        default:
            break;
    }
}

void MainWindow::changeQrcodeWidgetEx(int changeWidget, QList<int> newList)
{

    for(int i = 0;i< newList.count();i++)
    {
        changeQrcodeWidget(newList.at(i),true);
    }
    changeQrcodeWidget(changeWidget,false);

}

QString MainWindow::expendShowData(QString data,int mode)
{
    QString dataHear;
    dataHear.append(QString("<font style=' background-color:white; color:blue;'>%1 ["+QDateTime::currentDateTime().toString("yy-MM-dd hh:mm:ss.zzz")+"]</font>").arg((mode == 0) ? "[TX]":"[RX]"));
    QString newData = dataHear + data;
    return newData;
}

void MainWindow::qrcodeShowRules(int labelNumber,bool flag)
{
    if(flag) // 展开
    {
        int removeOne = 0;
        if(showQrcodeList.count() < 2 )
        {
            if(showQrcodeList.count() == 0 || (showQrcodeList.count() == 1 && showQrcodeList.at(0) != labelNumber))
            {
                showQrcodeList.append(labelNumber);
            }
        }
        else
        {
            removeOne = showQrcodeList.at(0);
            showQrcodeList.removeFirst();
            showQrcodeList.append(labelNumber);
        }
        changeQrcodeWidgetEx(removeOne,showQrcodeList);
    }
    else
    {
        int changeOne = 0;
        for(int i = 0;i<showQrcodeList.count();i++)
        {
            if(showQrcodeList.at(i) == labelNumber)
            {
                changeOne = labelNumber;
                showQrcodeList.removeAt(i);
            }
        }
        QList<int>totalList = {1,2,3};
        for(int i = 0;i<totalList.count();i++)
        {
            if(changeOne != totalList.at(i) && showQrcodeList.at(0) != totalList.at(i))
            {
                showQrcodeList.append(totalList.at(i));
            }
        }
        changeQrcodeWidgetEx(changeOne,showQrcodeList);
    }
}


void MainWindow::on_pushButton_check_clicked()
{
    if(!m_userConfig->deviceReady)
    {
        QMessageBox::critical(this,"错误","设备未就绪","确认");
        return;
    }
    else if(isChecking) // !isFirstCheck &&
    {
//        logTextEditDisplay(expendShowData("错误：当前设备正在检测中，请先结束本轮测试",SEND_TYPE));
//        return;
        if(NULL !=currentDialog)
        {
            currentDialog->close();
            currentDialog->deleteLater();
            currentDialog = NULL;
        }
    }
    if(m_userConfig->isCheckingEnd)
    {
        QMessageBox::critical(this,"错误","设备正在等待结束产测指令，请稍后","确认");
        return;
    }
    resetCheck();
    isChecking = true;
    logTextEditDisplay(expendShowData("设备开始检测..."));
    m_checkDeviceInfo->init();

    if(!((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolBasicData(1,CMD_PRODUCT_TEST_INIT))
    {
        qCritical()<<"send init mess error";
    }
    else
    {
        if(NULL !=serialAckTimer)
        {
            if(serialAckTimer->isActive())
            {
                serialAckTimer->setInterval(10*1000);
            }
            else
            {
                serialAckTimer->start(10*1000);
            }
        }
        m_userConfig->isCheckingInit = true;
    }
}

void MainWindow::on_commandLinkButton_config_clicked()
{
    if(!isChecking)
    {
        communiteConfigWidgetShow();
    }
}

void MainWindow::on_commandLinkButton_checkHistory_clicked()
{
    if(!isChecking)
    {
        saveRecord *newSaveRecord = new saveRecord();
        newSaveRecord->openSqlDataBase(m_userConfig->batch);
        newSaveRecord->showWidget(this->geometry());
    }
}

void MainWindow::recvSerialRecvData(QByteArray data)
{
    qDebug()<<"recvSerialRecvData:"<<data;
    if(!data.isEmpty())
    {
        logTextEditDisplay(expendShowData(data,RECV_TYPR));
    }
}

void MainWindow::on_checkBox_serialShow_clicked(bool checked)
{
    qDebug()<<"checked:"<<checked;
    if(checked)
    {
        if(!ui->groupBox_serialRecvDataShow->isHidden())
        {
            ui->groupBox_serialRecvDataShow->hide();
        }
    }
    else
    {
        if(ui->groupBox_serialRecvDataShow->isHidden())
        {
            ui->groupBox_serialRecvDataShow->show();
        }
    }
}



void MainWindow::on_pushButton_barCodeFold_clicked()
{

    if(0 == ui->pushButton_barCodeFold->text().compare("展开"))
    {
        qrcodeShowRules(1,true);
    }
    else
    {
        qrcodeShowRules(1,false);
    }
}

void MainWindow::on_pushButton_barCodeFold2_clicked()
{
    if(0 == ui->pushButton_barCodeFold2->text().compare("展开"))
    {
        qrcodeShowRules(2,true);
    }
    else
    {
        qrcodeShowRules(2,false);
    }
}

void MainWindow::on_pushButton_barCodeFold3_clicked()
{

    if(0 == ui->pushButton_barCodeFold3->text().compare("展开"))
    {
        qrcodeShowRules(3,true);
    }
    else
    {
        qrcodeShowRules(3,false);
    }
}

void MainWindow::on_comboBox_barCodeSeclect_currentIndexChanged(const QString &arg1)
{
    if(NULL != m_checkDeviceInfo && !m_checkDeviceInfo->IMEI.isEmpty())
    {
        if(0 == arg1.compare("二维码"))
        {
            GenerateQRcode(m_checkDeviceInfo->IMEI,ui->label_barCodeShow);
            changeQrcodeWidget(1,(0 == ui->pushButton_barCodeFold->text().compare("收起"))?true:false);
        }
        else
        {
            useBarcode *barcode = new useBarcode();
            barcode->setBarcodeAttribute(m_checkDeviceInfo->IMEI);
            QPixmap pixmap = barcode->saveImage();
            ui->label_barCodeShow->setFixedSize(pixmap.size());
            ui->label_barCodeShow->setPixmap(pixmap);
            changeQrcodeWidget(1,(0 == ui->pushButton_barCodeFold->text().compare("收起"))?true:false);
//            ui->label_barCodeShow->setPixmap(pixmap);
        }
    }
    else
    {

    }
}

/**************************************************************************
** 功能	@brief :  指令界面显示
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void MainWindow::logTextEditDisplay(QString data)
{

    if(!data.isEmpty())
    {
        qDebug()<<"logTextEditDisplay:"<<data;
        QRegExp valueRegExp(QString("(%1)").arg("\\[[TR]X\\]"));
        valueRegExp.setCaseSensitivity(Qt::CaseSensitive);
        data = data.replace(valueRegExp, "<font style=' background-color:white; color:blue;'>\\1</font>");
        ui->textEdit_serialRecvDataShow->append(data);
    }
    ui->textEdit_serialRecvDataShow->moveCursor(QTextCursor::End);
    ui->textEdit_serialRecvDataShow->update();

}

void MainWindow::sendSerialCheckEnd()
{
    QByteArray payload;
    if(m_userInfo.checkDeviceResult)
    {
        payload[0] = 0x01;
    }
    else
    {
        payload[0] = 0x00;
    }
    if(!((communiteConfigure *)m_comuniteConfigureFunc)->sendProtocolExpendData(1,CMD_PRODUCT_TEST_RESULT,payload))
    {
        qInfo()<<"发送结束产测指令失败";
    }
    else
    {
        logTextEditDisplay(expendShowData(QString("已发送结束产测指令")));
        m_userConfig->isCheckingEnd = true;
        if(NULL !=serialAckTimer)
        {
            if(serialAckTimer->isActive())
            {
                serialAckTimer->setInterval(10*1000);
            }
            else
            {
                serialAckTimer->start(10*1000);
            }
        }

    }
}

QString MainWindow::getTableWidgetItemResult(int row)
{
    QString ret = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
    switch(row)
    {
        case RESULT_PUSHBUTTON_LINE:
        if(NULL != ui->tableWidget_operate->cellWidget(RESULT_PUSHBUTTON_LINE,2))
        {
            if( 0 == ui->tableWidget_operate->cellWidget(RESULT_PUSHBUTTON_LINE,2)->toolTip().compare("成功"))
            {
                ret = RESULT_STATUS_STRING(1+true);
            }
            else if( 0 == ui->tableWidget_operate->cellWidget(RESULT_PUSHBUTTON_LINE,2)->toolTip().compare("失败"))
            {
                ret = RESULT_STATUS_STRING(1+false);
            }
        }
        break;
    case RESULT_REED_LINE:
        if(NULL != ui->tableWidget_operate->cellWidget(RESULT_REED_LINE,2))
        {
            if( 0 == ui->tableWidget_operate->cellWidget(RESULT_REED_LINE,2)->toolTip().compare("成功"))
            {
                ret = RESULT_STATUS_STRING(1+true);
            }
            else if( 0 == ui->tableWidget_operate->cellWidget(RESULT_PUSHBUTTON_LINE,2)->toolTip().compare("失败"))
            {
                ret = RESULT_STATUS_STRING(1+false);
            }
        }
        break;
    default:
        break;
    }
    return ret;
}

void MainWindow::action_handle(QAction *action)
{
    QString actionNmae = action->objectName();
#ifdef USE_UPDATE_INLINE
    if(0 == actionNmae.compare("action_updateVersion"))
    {
        if(NULL !=updateVersion)
        {
            ((toolUpdate *)updateVersion)->checkForUpdates();

        }
    }
#endif
}


