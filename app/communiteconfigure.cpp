#include "communiteconfigure.h"
#include "ui_communiteconfigure.h"
#include "kernel/useprinter.h"
#include "kernel/serial.h"
#include "kernel/serialdatahandle.h"
#include <QPrinterInfo>
#include <QPainter>
#include <QAbstractItemView>
#include <QCloseEvent>
#include "appConfig.h"
#include <QTime>
#include <QPixmap>

communiteConfigure::communiteConfigure(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::communiteConfigure)
{
    ui->setupUi(this);
    this->setWindowTitle("配置");
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(this->windowFlags()& ~Qt::WindowMinMaxButtonsHint);
    uiAttributeSet();
    initSerial();
    initPrinter();
    gc_recvTimer = new QTimer();
    connect(gc_recvTimer,SIGNAL(timeout()),this,SLOT(recvCheckTimeOut()));
//    QTimer *pTimer = new QTimer(this);
//    pTimer->singleShot(1000, this, [=](){historyRecover();});
}

communiteConfigure::~communiteConfigure()
{
    delete ui;
}

void communiteConfigure::widgetShow(QRect rect)
{
    if(NULL != gc_checkDeviceStatus)
    {
        gc_checkDeviceStatus->stop();
    }
    QTime dieTime = QTime::currentTime().addMSecs(300);

    while (QTime::currentTime() < dieTime) {
        if(isFirstUpdateSerialList)
        {
            break;
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    }
    QStringList printerList = QPrinterInfo::availablePrinterNames();
    ui->comboBox_printerSelect->clear();
    ui->comboBox_printerSelect->addItems(printerList);
    //调整下拉列表宽度，完整显示
    int max_len=0;
    for(int idx=0;idx<ui->comboBox_printerSelect->count();idx++)
    {
        if(max_len<ui->comboBox_printerSelect->itemText(idx).toLocal8Bit().length())
        {
            max_len = ui->comboBox_printerSelect->itemText(idx).toLocal8Bit().length();
        }
    }
    ui->comboBox_printerSelect->view()->setFixedWidth(max_len*9*0.75);
    historyRecover();
    this->move(rect.x()+rect.width()/2-this->width()/2,rect.y()+rect.height()/2-this->height()/2);
    this->show();
    gc_activityFlag = true;
}

void communiteConfigure::widgetHide()
{
    this->hide();
    gc_activityFlag = false;
}

bool communiteConfigure::sendProtocolBasicData(int pkid, int cmd)
{
    bool result = false;
    serialDataHandle handle;
    if(NULL != gc_qserial && ((qSerial *)gc_qserial)->serialIsOpen())
    {
        qDebug()<<"send data:"<<handle.sendBasicData(pkid,cmd).toHex();
        if(((qSerial *)gc_qserial)->SerialSend(handle.sendBasicData(pkid,cmd)) > 0)
        {
            result = true;
        }
    }
    return result;
}


bool communiteConfigure::sendProtocolExpendData(int pkid, int cmd,QByteArray payload)
{
    bool result = false;
    serialDataHandle handle;
    if(NULL != gc_qserial && ((qSerial *)gc_qserial)->serialIsOpen())
    {
        qDebug()<<"send data:"<<handle.sendExpandData(pkid,cmd,payload).toHex();
        if(((qSerial *)gc_qserial)->SerialSend(handle.sendExpandData(pkid,cmd,payload)) > 0)
        {
            result = true;
        }
    }
    return result;
}


bool communiteConfigure::printerPrintImage(QPixmap *image)
{
    if(NULL != gc_printer)
    {
        ((usePrinter *)gc_printer)->printPixmap(*image);
    }
    return true;
}

//QPixmap communiteConfigure::getBarcode(QString data)
//{

//}

void communiteConfigure::uiAttributeSet()
{
    int max_len=0;
    for(int idx=0;idx<ui->comboBox_printerSelect->count();idx++)
    {
        if(max_len<ui->comboBox_printerSelect->itemText(idx).toLocal8Bit().length())
        {
            max_len = ui->comboBox_printerSelect->itemText(idx).toLocal8Bit().length();
        }
    }
    ui->comboBox_printerSelect->view()->setFixedWidth(max_len*9*0.75);


    ui->pushButton_printerAttribute->hide();
    ui->pushButton_printPageAttribute->hide();
    ui->pushButton_printPreview->hide();

    ui->pushButton_cancel->setStyleSheet("QPushButton{background-color:white;color: black; border-radius: 10px;  border: 2px groove gray;border-style: outset;} \
                                          QPushButton:hover{background-color:rgba(153, 204, 255,250); color: black;border-style: inset;}    \
                                          QPushButton:pressed{background-color:rgb(51, 102, 255);border-style: inset; }");

    ui->pushButton_ok->setStyleSheet("QPushButton{background-color:rgb(0, 170, 255);color: black; border-radius: 10px;  border: 2px groove gray;border-style: outset;} \
                                     QPushButton:hover{background-color:rgba(153, 204, 255,250); color: black;border-style: inset;}    \
                                     QPushButton:pressed{background-color:rgb(51, 102, 255);border-style: inset; }");

//    ui->spinBox_printQuota->setMinimum(1);
    ui->spinBox_CSQMinValue->setRange(0,30);
    ui->spinBox_CSQMaxValue->setRange(1,31);
    ui->spinBox_CSQMaxValue->setValue(31);
    ui->radioButton_setCSQRange->setChecked(true);

    ui->comboBox_uartBate->setCurrentIndex(ui->comboBox_uartBate->count()-1);
    ui->comboBox_uartData->setCurrentIndex(ui->comboBox_uartData->count()-1);
    ui->comboBox_uartStop->setCurrentIndex(0);

}

void communiteConfigure::historyRecover()
{
    qDebug()<<"historyRecover";
    gc_appInfoSaveInstance = new appInfoSave("app_device_userInfo");
    int valueType = 0;
    QVariant value;
    if(gc_appInfoSaveInstance->findAppValue("app_serialInfo",&valueType,&value))
    {
        QStringList list =  value.toStringList();
        dataMatchCombox(ui->comboBox_uartPort,QString(list.at(0)));
        dataMatchCombox(ui->comboBox_uartBate,QString(list.at(1)));
        dataMatchCombox(ui->comboBox_uartData,QString(list.at(2)));
        dataMatchCombox(ui->comboBox_uartStop,QString(list.at(3)));
    }
    if(gc_appInfoSaveInstance->findAppValue("app_csqInfo",&valueType,&value))
    {
        qDebug()<<"valueType:"<<valueType;
        QStringList list =  value.toStringList();
        if(3 == list.size())
        {
            if(0 == QString(list.at(0)).toInt())
            {
                ui->radioButton_ignoreCSQ->setChecked(true);
            }
            else if(1 == QString(list.at(0)).toInt())
            {
                ui->radioButton_setCSQRange->setChecked(true);
                ui->spinBox_CSQMinValue->setValue(QString(list.at(1)).toInt());
                ui->spinBox_CSQMaxValue->setValue(QString(list.at(2)).toInt());
            }
        }
    }


    usePrinterInfoClass printerInfo;
    if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/name",&valueType,&value))
    {
        if(valueType == TYPE_STRING)
        {
            printerInfo.name = value.toString();
            dataMatchCombox(ui->comboBox_printerSelect,printerInfo.name);
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/copies",&valueType,&value))
        {
            printerInfo.copies = value.toInt();
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/pagOrder",&valueType,&value))
        {
            printerInfo.pagOrder = QPrinter::PageOrder(value.toInt());
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/pageSize",&valueType,&value))
        {
            printerInfo.pageSize = QPrinter::PageSize(value.toInt());
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/orientation",&valueType,&value))
        {
            printerInfo.orientation = QPrinter::Orientation(value.toInt());
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/paperSource",&valueType,&value))
        {
            printerInfo.paperSource = QPrinter::PaperSource(value.toInt());
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/left",&valueType,&value))
        {
            printerInfo.left = value.toInt();
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/top",&valueType,&value))
        {
            printerInfo.top = value.toInt();
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/right",&valueType,&value))
        {
            printerInfo.right = value.toInt();
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/bottom",&valueType,&value))
        {
            printerInfo.bottom = value.toInt();
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/unit",&valueType,&value))
        {
            printerInfo.unit = value.toInt();
        }
        if(gc_appInfoSaveInstance->findAppValue("app_printerInfo/colorMode",&valueType,&value))
        {
            qDebug()<<"1... value.toInt()"<<value.toInt();
            printerInfo.colorMode = value.toInt();
        }
        ((usePrinter *)gc_printer)->initPrinter(printerInfo);

    }


}


void communiteConfigure::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

}

void communiteConfigure::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
}

void communiteConfigure::initSerial()
{
    if(NULL == gc_qserial)
    {
        this->gc_qserial = new qSerial();
        ((qSerial *)gc_qserial)->portListPeriodSet(50);
        connect((qSerial *)gc_qserial,SIGNAL(portListNoticSignal(QList<QString>)),this,SLOT(portListNoticSlot(QList<QString>)));
        connect((qSerial *)gc_qserial,SIGNAL(dataReadNoticSignal(QByteArray)),this,SLOT(dataReadNoticSlot(QByteArray)));
    }
}

void communiteConfigure::initPrinter()
{
    if(NULL == gc_printer)
    {
        gc_printer = new usePrinter();
    }
}

bool communiteConfigure::openSerial()
{
    // 第二版可以优化:根据serial的修改才去重新打开
    if(gc_serialOpenFlag)
    {
        ((qSerial *)gc_qserial)->SerialClose();
    }
    if(!((qSerial *)gc_qserial)->serialOpen(ui->comboBox_uartPort->currentText(),ui->comboBox_uartBate->currentText().toInt(),ui->comboBox_uartData->currentText().toInt(),QSerialPort::NoParity,ui->comboBox_uartStop->currentText().toInt(),QSerialPort::NoFlowControl))
    {
        gc_serialOpenFlag = false;
    }
    else
    {
        gc_serialOpenFlag = true;
    }
    return gc_serialOpenFlag;
}

QByteArray communiteConfigure::splitDataLine(QByteArray data,int *outLen)
{
    qInfo()<<__FUNCTION__;
    QByteArray outData;
    int pEnd = 0;
    int offset = 0;
    if((pEnd =data.indexOf("\r\n"))>= 0)
    {
        offset = 2;
    }
    else if((pEnd =data.indexOf("\r")>= 0))
    {
        offset = 0;
    }
    else if((pEnd =data.indexOf("\n")>= 0))
    {
        offset = 1;
    }
    if((pEnd < 0))
    {
        return NULL;
    }
    *outLen = pEnd+offset;
    for (int i = 0; i < *outLen; i++)
    {
        outData.append(data.at(i));
    }
    return outData;

}


/**************************************************************************
* @brief    打印机更多设置的控件表现方式
* @param [in]
* @param [out]
* @retval
***************************************************************************/
void communiteConfigure::on_pushButton_printerMoreAttribute_clicked()
{
    if(ui->pushButton_printerMoreAttribute->text().contains("更多设置"))
    {
        ui->pushButton_printerAttribute->show();
        ui->pushButton_printPageAttribute->show();
        ui->pushButton_printerMoreAttribute->setText("︽");
    }
    else
    {
        ui->pushButton_printerAttribute->hide();
        ui->pushButton_printPageAttribute->hide();
        ui->pushButton_printerMoreAttribute->setText("更多设置");
    }
}

/**************************************************************************
* @brief        csq最大值的改变
* @param [in]   csq最大值
* @param [out]
* @retval
***************************************************************************/
void communiteConfigure::on_spinBox_CSQMinValue_valueChanged(int arg1)
{
    if(arg1 < 31)
    {
        ui->spinBox_CSQMaxValue->setMinimum(arg1+1);
        g_csqMinValue = arg1;
    }
}

/**************************************************************************
* @brief        是否采用csq的自限制方式
* @param [in]
* @param [out]
* @retval
***************************************************************************/
void communiteConfigure::on_radioButton_setCSQRange_toggled(bool checked)
{
    if(checked)
    {
        ui->spinBox_CSQMinValue->setEnabled(true);
        ui->spinBox_CSQMaxValue->setEnabled(true);
    }
    else
    {
        ui->spinBox_CSQMinValue->setEnabled(false);
        ui->spinBox_CSQMaxValue->setEnabled(false);
    }
}
/**************************************************************************
* @brief        打印机属性设置
* @param [in]
* @param [out]
* @retval
***************************************************************************/
void communiteConfigure::on_pushButton_printerAttribute_clicked()
{
    if((NULL != gc_printer))
    {
        if(((usePrinter *)gc_printer)->getCurrentPrinterName().isEmpty())
        {
            if(false == ((usePrinter *)gc_printer)->initPrinter(ui->comboBox_printerSelect->currentText()))
            {
                QMessageBox::critical(this,"错误","打开打印机失败","确认");
                return;
            }
        }
        ((usePrinter *)gc_printer)->setPrinterAttribute();
    }
    else
    {
        qDebug()<<"on_pushButton_printerAttribute_clicked :error init ";
    }
}
/**************************************************************************
* @brief        打印机页面属性设置
* @param [in]
* @param [out]
* @retval
***************************************************************************/
void communiteConfigure::on_pushButton_printPageAttribute_clicked()
{
    if((NULL != gc_printer))
    {
        if(((usePrinter *)gc_printer)->getCurrentPrinterName().isEmpty())
        {
            if(false == ((usePrinter *)gc_printer)->initPrinter(ui->comboBox_printerSelect->currentText()))
            {
                QMessageBox::critical(this,"错误","打开打印机失败","确认");
                return;
            }
        }
        ((usePrinter *)gc_printer)->setPrinterPageAttribute();
    }
}

void communiteConfigure::on_pushButton_printPreview_clicked()
{
//    usePrinter myPainter;
//    QPixmap pixmap = QPixmap::grabWidget(ui->tabWidget_serial, ui->tabWidget_serial->rect());
//    myPainter.startPrintPreview(pixmap);
}


void communiteConfigure::portListNoticSlot(QList<QString> portList)
{
    qDebug()<<__FUNCTION__;
    if(portList.count() <= 0)
    {
        qWarning()<<"串口列表为空";
        return;
    }
    QString currentSerialName = ((qSerial *)gc_qserial)->getCurrentSerialName();
    if(((qSerial *)gc_qserial)->serialIsOpen() && !currentSerialName.isEmpty())
    {
        if(!portList.contains(currentSerialName))
        {
            resetSerial();
            QStringList list;
            list<<QString::number(false)<<QString::number(true);
            emit signalDeviceStatus(list);
        }
    }
    ui->comboBox_uartPort->clear();
    ui->comboBox_uartPort->addItems(portList);
    //调整下拉列表宽度，完整显示
    int max_len=0;
    for(int idx=0;idx<ui->comboBox_uartPort->count();idx++)
    {
        if(max_len<ui->comboBox_uartPort->itemText(idx).toLocal8Bit().length())
        {
            max_len = ui->comboBox_uartPort->itemText(idx).toLocal8Bit().length();
        }
    }
    ui->comboBox_uartPort->view()->setFixedWidth(max_len*9*0.75);
    isFirstUpdateSerialList = true;
}

void communiteConfigure::dataReadNoticSlot(QByteArray data)
{
    qDebug()<<__FUNCTION__;
    gc_recvData.append(data);
    if(gc_recvTimer->isActive())
    {
        gc_recvTimer->setInterval(200);
    }
    else
    {
        gc_recvTimer->start(200);
    }
}

void communiteConfigure::checkDeviceStatusTimeout()
{
    QStringList list;
    bool needSendFlag = false;
    if(!((qSerial *)gc_qserial)->serialIsOpen())
    {
        list<< QString::number(false);
        needSendFlag = true;
        resetSerial();
        qDebug()<<"serial close";
    }
    else
    {
        list<< QString::number(true);
    }
    if(false == gc_activityFlag && ((usePrinter *)gc_printer)->getCurrentPrinterName() != ui->comboBox_printerSelect->currentText())
    {
        list<< QString::number(false);
        needSendFlag = true;
        resetPrinter();
    }
    else
    {
        list<< QString::number(true);
    }
    if(needSendFlag)
    {
        emit signalDeviceStatus(list);
    }
}

void communiteConfigure::recvCheckTimeOut()
{
    qDebug()<<__FUNCTION__<<"gc_recvHandleFlag:"<<gc_recvHandleFlag;
    if(!gc_recvData.isEmpty())
    {
        if(false == gc_recvHandleFlag)
        {
            dataHandle(gc_recvData);
        }
        else
        {
            gc_recvTimer->start(200);
        }
    }
    else
    {
        gc_recvTimer->stop();
    }
}



void communiteConfigure::dataHandle(QByteArray &data)
{
    qDebug()<<__FUNCTION__<<data;
    gc_recvHandleFlag = true;
//    int handleDataLen = 0;
    if(!data.isEmpty())
    {
//        QByteArray lineData = splitDataLine(gc_recvData,&handleDataLen);
        serialDataAnalysis(gc_recvData);
        gc_recvData.clear();
    }
    gc_recvHandleFlag = false;
}

void communiteConfigure::on_pushButton_ok_clicked()
{
    if(!openSerial())
    {
        QMessageBox::critical(this,"错误","打开串口失败","确认");
        return;
    }
    else
    {
        QStringList list;
        list<<ui->comboBox_uartPort->currentText()<<ui->comboBox_uartBate->currentText()<<ui->comboBox_uartData->currentText()<<ui->comboBox_uartStop->currentText();
        if(NULL != gc_appInfoSaveInstance)
        {
            gc_appInfoSaveInstance->modifyAppInfo("app_serialInfo",TYPE_STRINGLIST,list);
        }
    }

    if(false == ((usePrinter *)gc_printer)->initPrinter(ui->comboBox_printerSelect->currentText()))
    {
        QMessageBox::critical(this,"错误","打开打印机失败","确认");
        return;
    }
    else
    {
        usePrinterInfoClass printerInfo=((usePrinter *)gc_printer)->getPrinterInfo();
        QJsonObject configObj;
        configObj.insert("name",printerInfo.name);
        configObj.insert("copies",printerInfo.copies);
        configObj.insert("pagOrder",printerInfo.pagOrder);
        configObj.insert("pageSize",printerInfo.pageSize);
        configObj.insert("orientation",printerInfo.orientation);
        configObj.insert("paperSource",printerInfo.paperSource);
        configObj.insert("left",printerInfo.left);
        configObj.insert("top",printerInfo.top);
        configObj.insert("right",printerInfo.right);
        configObj.insert("bottom",printerInfo.bottom);
        configObj.insert("unit",printerInfo.unit);
        configObj.insert("colorMode",printerInfo.colorMode);
        if(NULL != gc_appInfoSaveInstance)
        {
            gc_appInfoSaveInstance->modifyAppInfo("app_printerInfo",TYPE_JOSN_OBJECT,configObj);
        }

    }
    QStringList list;
    if(ui->radioButton_setCSQRange->isChecked())
    {
        list<<QString::number(g_csqMinValue)<<QString::number(g_csqMaxValue);
    }
    else
    {
        list<<QString::number(0)<<QString::number(31);
    }
    QStringList saveCsqInfo;
    saveCsqInfo<<QString::number(ui->radioButton_setCSQRange->isChecked())<<list;
    if(NULL != gc_appInfoSaveInstance)
    {
        gc_appInfoSaveInstance->modifyAppInfo("app_csqInfo",TYPE_STRINGLIST,saveCsqInfo);
    }
    emit openDeviceSuccess(list);
    if(NULL == gc_checkDeviceStatus)
    {
        gc_checkDeviceStatus = new QTimer();
        connect(gc_checkDeviceStatus,SIGNAL(timeout()),this,SLOT(checkDeviceStatusTimeout()));
        gc_checkDeviceStatus->start(5000);
    }
    else
    {
        gc_checkDeviceStatus->start(5000);
    }
}

void communiteConfigure::on_pushButton_cancel_clicked()
{
    this->hide();
}


//class cmdList_type
//{
//public:
//    cmdList_type(int cmdType,void (communiteConfigure::*cFun)(QStringList list) )
//    {
//        cmd=cmdType;
//        fun=cFun;
//    }
//    int cmd;
//    void (communiteConfigure::*fun)(QStringList list);
//};

//QList<cmdList_type>handleCmdList
//{
//    {CMD_PRODUCT_TEST_INIT_ACK ,&communiteConfigure::cmd_PtInitAck_process },

//};

#include <QTextCodec>
bool communiteConfigure::serialDataAnalysis(QByteArray data)
{
    qDebug()<<__FUNCTION__;
    if(data.size() < 9)
    {
        return false;
    }
    serialDataHandle handle;
    QByteArray decodeData;
    bool ret = handle.serialDataAnalysis(data.toHex(),&decodeData);
    emit signalSerialRecvData(decodeData.toHex());

    if(ret)
    {
        int protoolCmd = ((decodeData[7]<<8)&0xFF00) + (decodeData[8] &0xFF);
        QByteArray payload;
        if(decodeData.count()>9)
        {
            payload= decodeData.mid(9);
        }
        qDebug()<<"protoolCmd:"<<protoolCmd;
        qDebug()<<"decodeData[7]:"<<QString::number(decodeData[7])<<",decodeData[8]:"<<QString::number(decodeData[8]);
        switch(protoolCmd)
        {
            case CMD_PRODUCT_TEST_INIT_ACK:
            {
                QStringList list;
                emit signalWidgetChange(RESULT_CHECK_INIT,list);
                break;
            }
            case CMD_MODULE_INFO_GET_ACK:
            {
                QStringList list = QString(payload).split("&");
                emit signalWidgetChange(RESULT_MODULE_INFO_LINE,list);
                break;
            }
            case CMD_PRODUCT_TEST_RESULT_ACK:
            {
                qInfo()<<"收到结束产测指令的回馈";
                QStringList list;
                emit signalWidgetChange(RESULT_CHECK_END,list);
                break;
            }
            case CMD_LED_CHECK_ACK:
            case CMD_BEER_CHECK_ACK:
            case CMD_PUSHBUTTON_CHECK_ACK:
            case CMD_REED_CHECK_ACK:
            {
                QStringList list;
                if(payload.size() > 0)
                {
                    list<<QString::number(payload[0]);
                }
                emit signalWidgetChange((protoolCmd-CMD_LED_CHECK_ACK)/2+RESULT_LED_LINE,list);
                break;
            }
            case CMD_INTERNET_CHECK_ACK:
            {
//                QByteArrayList arrayList= payload.split('&');
//                QStringList list;
//                for(int i = 0;i<arrayList.size();i++)
//                {
//                    list<<QString().toInt();
//                }

                QByteArrayList byteList = payload.split('&');
                qDebug()<<"byteList:"<<byteList;
                QStringList list;
                for(int i = 0;i<byteList.count();i++)
                {
                    QByteArray byte = byteList.at(i).toHex();
                    list.append(QString::number(byte.toUInt(nullptr,16)));
                }
                qDebug()<<"list:"<<list;
                emit signalWidgetChange(RESULT_INTERNET_LINE,list);
                break;
            }
            default:
                break;

        }
        return true;
    }
    return false;
}

void communiteConfigure::dataMatchCombox(QWidget *widget, QString data)
{
    qDebug()<<"data:"<<data;
    for(int i = 0;i< ((QComboBox *)widget)->count();i++)
    {
        if(0 == ((QComboBox *)widget)->itemText(i).compare(data))
        {
            ((QComboBox *)widget)->setCurrentIndex(i);
            break;
        }
    }
}

void communiteConfigure::resetSerial()
{
    ui->comboBox_uartPort->setCurrentIndex(0);
    ui->comboBox_uartBate->setCurrentIndex(ui->comboBox_uartBate->count()-1);
    ui->comboBox_uartData->setCurrentIndex(ui->comboBox_uartData->count()-1);
    ui->comboBox_uartStop->setCurrentIndex(0);
}

void communiteConfigure::resetPrinter()
{
    usePrinterInfoClass printerInfo;
    ((usePrinter *)gc_printer)->initPrinter(printerInfo);
    ui->comboBox_printerSelect->setCurrentIndex(0);
}

void communiteConfigure::on_pushButton_configReset_clicked()
{
    // 清除记录文件
    if(NULL != gc_appInfoSaveInstance)
    {
        gc_appInfoSaveInstance->clearFile();
    }
    // 初始化打印机
    resetPrinter();

    // 初始化串口
    resetSerial();

    ui->radioButton_setCSQRange->setChecked(true);
    ui->spinBox_CSQMinValue->setValue(0);
    ui->spinBox_CSQMaxValue->setValue(31);

}

void communiteConfigure::on_spinBox_CSQMaxValue_valueChanged(int arg1)
{
    g_csqMaxValue = arg1;
}
