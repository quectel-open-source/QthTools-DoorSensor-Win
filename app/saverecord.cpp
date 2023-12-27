#include "saverecord.h"
#include "ui_saverecord.h"
#include "kernel/mysqlite.h"
#include "kernel/kernelconfig.h"
#include "appConfig.h"
#include <QFileDialog>
#include <QtXlsx>
saveRecord::saveRecord(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::saveRecord)
{
    ui->setupUi(this);
    this->setWindowTitle("移远门磁产测");
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowIcon(QIcon(":/image/quectel.ico"));
    this->setWindowFlags(this->windowFlags() | Qt::WindowMaximizeButtonHint);
    currentUserInfo = new saveRecord_userInfo();
    changeUserInfo = new saveRecord_userInfo();
    currentUserInfo->init();
    changeUserInfo->init();
    uiAttributeSet();
    initSqlDataBase();
}

saveRecord::~saveRecord()
{
    delete ui;
}

void saveRecord::uiAttributeSet()
{
    ui->dateEdit_startTime->setDateTime(QDateTime::currentDateTime());
    ui->dateEdit_endTime->setDateTime(QDateTime::currentDateTime());
    if(ui->comboBox_batch->count() > 0)
    {
        currentUserInfo->deviceBatch = ui->comboBox_batch->currentText();
    }
    currentUserInfo->checkStartTime = ui->dateEdit_startTime->dateTime().toTime_t();
    currentUserInfo->checkEndTime = ui->dateEdit_endTime->dateTime().toTime_t();
    currentUserInfo->checkResult = ui->comboBox_resultSelect->currentIndex();

//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(ui->tableWidget->horizontalHeader()->count()-1,QHeaderView::Stretch);
//    ui->tableWidget->setColumnWidth(ui->tableWidget->horizontalHeader()->count()-1,100);
//    ui->tableWidget->resizeRowsToContents();

    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHorizontal_Mask);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->dateEdit_endTime->setMinimumDate(ui->dateEdit_startTime->date());


}

void saveRecord::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
}

void saveRecord::initSqlDataBase()
{
    // 打开设备管理数据库
    recordOfManageSql = new mySqlite();
    ((mySqlite *)recordOfManageSql)->createSqlDataBase("recordManager","recordManager");
    // 新建/打开表
    if(NULL !=recordOfManageSql)
    {
        ((mySqlite *)recordOfManageSql)->createSqlQuery("recordManager", \
            "create table recordManager (batch TEXT(32) primary key,filePath TEXT(120))");
        // 创建索引
        // ((mySqlite *)userInfoSql)->execSqlDataList("CREATE INDEX index_name ON device_userInfo (dk);");
    }
    // 打开当前设备数据库(当前批次+当前时间)
    QStringList list = ((mySqlite *)recordOfManageSql)->execSqlDataList(QString("SELECT batch FROM recordManager;"));
    if(list.size() > 0)
    {
        ui->comboBox_batch->clear();
        ui->comboBox_batch->addItems(list);
    }
}

void saveRecord::tableToTableWidghtShow(int mode, QString totalResult, bool needTips, int selectDay)
{

    // 显示当天的数据
    int days =0;
    if(-1 == selectDay)
    {
        days= m_getTimeStamp(QDate::currentDate());
    }
    else
    {
        days = selectDay;
    }
    // 如果存在当天记录则显示
    if(((mySqlite *)recordOfBatchSql)->findTableName(createTableName(days)))
    {
        if(0 == mode)
        {
            sqlHistoryShow(createTableName(days),totalResult);
        }
        else
        {
            sqlHistoryShow(createTableName(days),totalResult,1);
        }
    }
    else
    {
        if(needTips)
        {
            QMessageBox::information(this,"温馨提示",QDate::currentDate().toString()+"无记录，如需查询历史请选择相应日期","确认");
        }
    }
}

void saveRecord::sqlHistoryShow(QString tableName, QString totalResult, int mode)
{
    QStringList countList = ((mySqlite *)recordOfBatchSql)->execSqlDataList(QString("SELECT count(imei) AS number FROM '%1';").arg(tableName));
    int counts = QString(countList.at(0)).toInt();
    int i = 0;
    int tableWidgetCount = 0;
    if(counts > 0)
    {
        if(0 == mode)
        {
//            ui->tableWidget->setRowCount(counts);
        }
        else if (1 == mode)
        {
            tableWidgetCount = ui->tableWidget->rowCount();
//            ui->tableWidget->setRowCount(counts+i);
        }
    }
    else
    {
        return;
    }
    if(i/1024 >0)
    {
        for(;i<counts;i+=1024)
        {
            QStringList dataList;
            dataList = ((mySqlite *)recordOfBatchSql)->getSqlDataList(QString("SELECT * FROM %1 limit %2,%3").arg(tableName).arg(i).arg(1024));


            if(dataList.size() > 0)
            {
                for(int dataList_i = 0;dataList_i < dataList.size();dataList_i++)
                {
                    QStringList cordList = QString(dataList.at(dataList_i)).split(",");
                    if((cordList.count() == 14))
                    {

                        if(!totalResult.isEmpty())
                        {

                            if(0 ==QString(cordList[13]).compare(totalResult))
                            {
                                insertTableWidget(cordList,tableWidgetCount++);
                            }
                        }
                        else
                        {
                            insertTableWidget(cordList,tableWidgetCount++);
                        }
                    }
                }
            }
        }
    }
    int residual = counts%1024;
    if( residual > 0)
    {
        QStringList dataList;
//        if(totalResult.isEmpty())
//        {
           dataList = ((mySqlite *)recordOfBatchSql)->getSqlDataList(QString("SELECT * FROM %1 limit %2,%3").arg(tableName).arg(i-1024).arg(residual));
           qDebug()<<"dataList:"<<dataList;
//        }
//        else
//        {
//            dataList = ((mySqlite *)recordOfBatchSql)->getSqlDataList(QString("SELECT * FROM %1 where result='%2' limit %3,%4").arg(tableName).arg(totalResult).arg(i-1024).arg(residual));
//        }
        if(dataList.size() > 0)
        {
            for(int dataList_i = 0;dataList_i < dataList.size();dataList_i++)
            {

                QStringList cordList = QString(dataList.at(dataList_i)).split(",");

                if((cordList.count() == 14))
                {
                    if(!totalResult.isEmpty())
                    {
                        if(0 ==QString(cordList[13]).compare(totalResult))
                        {
                            insertTableWidget(cordList,tableWidgetCount++);
                        }
                    }
                    else
                    {
                        insertTableWidget(cordList,tableWidgetCount++);
                    }
                }

            }
        }
    }
}

bool saveRecord::openSqlDataBase(QString batch)
{
    bool isHad = false;
    for(int i = 0;i< ui->comboBox_batch->count();i++)
    {
        if(0 == ui->comboBox_batch->itemText(i).compare(batch))
        {
            isHad = true;
        }
    }
    if(false == isHad)
    {
        QString insertTable = QString("insert into recordManager values('%1','%2');").arg(batch).arg("./");
        int errorNum = 0;
        ((mySqlite *)recordOfManageSql)->insertSqlData(insertTable,&errorNum);
        if(NULL != recordOfBatchSql)
        {
            delete recordOfBatchSql;
            recordOfBatchSql = NULL;
        }
        recordOfBatchSql = new mySqlite();
        ((mySqlite *)recordOfBatchSql)->createSqlDataBase(batch,batch);  // QString("%1/recordOfBatch").arg(batch)

    ((mySqlite *)recordOfBatchSql)->createSqlQuery(createTableName(m_getTimeStamp(QDate::currentDate())), \
        QString("create table %1 (imei TEXT(16) primary key,moduleType TEXT(16),batch TEXT(32),sn TEXT(32),person TEXT(16),checkTime TEXT(16),moduleVersion TEXT(30),LedResult TEXT(2),"\
                "beerReult TEXT(2),pushButtonResult TEXT(2),reedResult TEXT(2),internelResult TEXT(2),csqResult TEXT(2),result TEXT(6)" \
                ")").arg(createTableName(m_getTimeStamp(QDate::currentDate()))));
        ui->comboBox_batch->insertItem(0,batch);
//        ui->comboBox_batch->setCurrentIndex(0);
    }
    else
    {
        if(NULL != recordOfBatchSql)
        {
            delete recordOfBatchSql;
            recordOfBatchSql = NULL;
        }
        recordOfBatchSql = new mySqlite();
        ((mySqlite *)recordOfBatchSql)->createSqlDataBase(batch,batch);  // QString("%1/recordOfBatch").arg(batch)

    ((mySqlite *)recordOfBatchSql)->createSqlQuery(createTableName(m_getTimeStamp(QDate::currentDate())), \
        QString("create table %1 (imei TEXT(16) primary key,moduleType TEXT(16),batch TEXT(32),sn TEXT(32),person TEXT(16),checkTime TEXT(16),moduleVersion TEXT(30),LedResult TEXT(2),beerReult TEXT(2),pushButtonResult TEXT(2),reedResult TEXT(2),"\
                "internelResult TEXT(2),csqResult TEXT(2),result TEXT(6));").arg(createTableName(m_getTimeStamp(QDate::currentDate()))));
    }
    qDebug()<<"batch"<<batch;
    changeUserInfo->deviceBatch = batch;
    ui->comboBox_batch->setCurrentText(changeUserInfo->deviceBatch);
    return true;
}

bool saveRecord::insertHistoryToSql(QStringList list)
{
    if(NULL != recordOfBatchSql || list.size() == 14)
    {
//        int listCount = list.size();
//        switch (listCount) {
//        case 1:

//            break;
//        default:
//            break;
//        }
         qDebug()<<"insertHistoryToSql:"<<list;

        QString insertInfo = packageSqlString(list);
        int errorNum = 0;
        if(((mySqlite *)recordOfBatchSql)->insertSqlData(insertInfo,&errorNum))
        {
            return true;
        }
        else
        {
             qCritical()<<"insert error,num:"<<errorNum;
        }
    }
    return false;
}

bool saveRecord::modifyHistoryToSql(QStringList list)
{
    QString modifyInfo = packageSqlString(list,1);
    if(((mySqlite *)recordOfBatchSql)->modifySqlData(modifyInfo))
    {
        return true;
    }
    else
    {
         qCritical()<<"modify error";
    }
    return false;
}

bool saveRecord::isOpenTable()
{
    if(changeUserInfo->deviceBatch.isEmpty())
    {
        return false;
    }
    return true;
}

void saveRecord::showWidget(QRect rect)
{
    if(!changeUserInfo->deviceBatch.isEmpty())
    {
        QString totalReSult;
        tableToTableWidghtShow(0,totalReSult);
    }

    this->move(rect.x()+rect.width()/2-this->width()/2,rect.y()+rect.height()/2-this->height()/2);
    this->show();
}





void saveRecord::insertTableWidget(QStringList data,int insertRow)
{
    if(data.size() != 14)
    {
        qDebug()<<"sql数据异常,"<<"size:"<<data.size();
        qDebug()<<"data:"<<data;
        return;
    }
    int column = 0;
    for(int i =0;i<data.size();i++)
    {
        if(QString(data.at(i)).isEmpty())
        {
            data[i] = "-";
        }
    }
    if(insertRow+1 >= ui->tableWidget->rowCount())
    {
        ui->tableWidget->setRowCount(insertRow+1);
    }
    QTableWidgetItem *item_moduleType = new QTableWidgetItem(data[1]);
    item_moduleType->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_moduleType);

    QTableWidgetItem *item_batch = new QTableWidgetItem(data[2]);
    item_batch->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_batch);

    QTableWidgetItem *item_imei = new QTableWidgetItem(data[0]);
    item_imei->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_imei);

    QTableWidgetItem *item_sn = new QTableWidgetItem(data[3]);
    item_sn->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_sn);


    QTableWidgetItem *item_person = new QTableWidgetItem(data[4]);
    item_person->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_person);

    QTableWidgetItem *item_checkTime = new QTableWidgetItem(data[5]);
    item_checkTime->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_checkTime);

    QTableWidgetItem *item_moduleVer = new QTableWidgetItem(data[6]);
    item_moduleVer->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_moduleVer);

    QTableWidgetItem *item_led = new QTableWidgetItem(data[7]);
    item_led->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_led);

    QTableWidgetItem *item_beer = new QTableWidgetItem(data[8]);
    item_beer->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_beer);

    QTableWidgetItem *item_pushButton = new QTableWidgetItem(data[9]);
    item_pushButton->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_pushButton);

    QTableWidgetItem *item_reed = new QTableWidgetItem(data[10]);
    item_reed->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_reed);

    QTableWidgetItem *item_internet = new QTableWidgetItem(data[11]);
    item_internet->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_internet);

    QTableWidgetItem *item_csq = new QTableWidgetItem(data[12]);
    item_csq->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_csq);

    QTableWidgetItem *item_result = new QTableWidgetItem(data[13]);
    item_result->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(insertRow,column++,item_result);

}

QString saveRecord::createTableName(int64_t days)
{
    return "T"+QString::number(days);
}

void saveRecord::saveTabDataToFile(QString fileName, bool noNeedTip)
{
    //打开文件
//    QFile file(fileName);
//    if(false == file.open(QIODevice::WriteOnly))
//    {
//        QMessageBox::critical(this,QString::fromLocal8Bit("错误"),QString::fromLocal8Bit("文件保存失败"),QString::fromLocal8Bit("确认"));
//        return;
//    }
//    else
//    {
//        QTextStream streamFileOut(&file);
//        streamFileOut.setCodec("UTF-8");
//        // 写入标题
//        if( 0 == file.size() )
//        {
////            QStringList list;
////            streamFileOut<<stackWidgetInfo.currentWidget->headerTextListToString(list)+"\r\n";
////            streamFileOut.flush();
//        }
//        // 保存历史记录
//        m_currentSavePath = fileName;
//    }
////    qDebug()<<"saveTablewidgetData";
////    if(false == saveTablewidgetData(file))
////    {
////        file.close();
////        QFile::remove(fileName);
////        return;
////    }
    QXlsx::Document xlsx(fileName);
    bool isErrorHeader = false;
    if(xlsx.dimension().rowCount() > 0)
    {
        qDebug()<<"xls rowCount:"<<xlsx.dimension().rowCount();
        if(xlsx.read("A1").toString().isEmpty())
        {
            isErrorHeader = true;
        }
        else
        {
            QStringList list;
            list.append(xlsx.read("A1").toString());
            if(!matchFileHeader(list,1))
            {
                isErrorHeader = true;
            }
        }
    }
    else
    {
        isErrorHeader = true;
    }
    if(isErrorHeader)
    {
        qDebug()<<"set header";
        QXlsx::Format headerFormat;
        headerFormat.setFontBold(true);
        if(false == xlsx.write("A1","*设备编号",headerFormat))
        {
            qDebug()<<"写入标题失败";
        }
    }
    for(int i = 0;i<ui->tableWidget->rowCount();i++)
    {
//        if(!ui->tableWidget->item(i,13)->text().isEmpty())
//        {
//            if(0 == ui->tableWidget->item(i,13)->text().compare("不通过"))
//            {
//                headerFormat.setFontColor(QColor(255,0,0));
//            }
//        }
        if(0 == ui->tableWidget->item(i,13)->text().compare("通过"))
        {
            if(!xlsx.write(i+2,1,ui->tableWidget->item(i,2)->text()))
            {
                qDebug()<<"写入第"<<i<<"行数据时失败";
            }
        }
    }
    xlsx.save();

    if(noNeedTip)
    {
        QMessageBox::information(this,"提示","文件导出成功","确认");
    }
    //    file.close();
}

void saveRecord::saveTabDataListToFile(QString fileName, bool noNeedTip)
{
    qDebug()<<__FUNCTION__;
    QXlsx::Document xlsx(fileName);
    int tabWidgetColums = ui->tableWidget->columnCount();
    if(xlsx.dimension().rowCount() > 0)
    {
        qDebug()<<"xls rowCount:"<<xlsx.dimension().rowCount();
        QStringList list;
        for(int i=0;i<xlsx.dimension().columnCount();i++)
        {
            list.append(xlsx.read(1,i+1).toString());
        }

        if(!matchFileHeader(list))
        {
            qDebug()<<"set header";
            QXlsx::Format headerFormat;
            headerFormat.setFontBold(true);
            for(int i=0;i<tabWidgetColums;i++)
            {
                if(false == xlsx.write(1,i+1,ui->tableWidget->horizontalHeaderItem(i)->text()))
                {
                    qDebug()<<QString("写入第%1列标题失败").arg(i+1);
                }
            }
        }
    }
    else
    {
        for(int i=0;i<tabWidgetColums;i++)
        {
            if(false == xlsx.write(1,i+1,ui->tableWidget->horizontalHeaderItem(i)->text()))
            {
                qDebug()<<QString("写入第%1列标题失败").arg(i+1);
            }
        }
    }
    QXlsx::Format dataFormat;
//    dataFormat.setVerticalAlignment(QXlsx::Format::AlignHCenter);
    dataFormat.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    for(int i = 0;i<ui->tableWidget->rowCount();i++)
    {
        for(int j = 0;j<tabWidgetColums;j++)
        {
//            qDebug()<<"i:"<<i<<",j:"<<j;
            if(!xlsx.write(i+2,j+1,ui->tableWidget->item(i,j)->text(),dataFormat))
            {
                qDebug()<<QString("写入第%1行第%2列数据时失败").arg(i).arg(j);
            }
        }
    }
    xlsx.save();

    if(noNeedTip)
    {
        QMessageBox::information(this,"提示","文件导出成功","确认");
    }
}

bool saveRecord::matchFileHeader(QStringList list, int mode)
{
    if(1 == mode)
    {
        return (0 == QString(list[0]).compare("*设备编号"))?true:false;
    }
    else
    {
        bool result = true;
        for(int i=0;i<ui->tableWidget->columnCount();i++)
        {
            if(0 != ui->tableWidget->item(0,i)->text().compare(list[i]))
            {
                result = false;
                break;
            }
        }
        return result;
    }
}

QString saveRecord::packageSqlString(QStringList list, int mode)
{
    QString insertInfo;
    if(list.count() <14)
    {
        return insertInfo;
    }
    if(0 == mode)
    {
        insertInfo = QString("insert into '%1' values('%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15');")
                .arg(createTableName(m_getTimeStamp(QDate::currentDate())))
                .arg(QString(list[0]))
                .arg(QString(list[1]))
                .arg(QString(list[2]))
                .arg(QString(list[3]))
                .arg(QString(list[4]))
                .arg(QString(list[5]))
                .arg(QString(list[6]))
                .arg(QString(list[7]))
                .arg(QString(list[8]))
                .arg(QString(list[9]))
                .arg(QString(list[10]))
                .arg(QString(list[11]))
                .arg(QString(list[12]))
                .arg(QString(list[13]));
    }
    else
    {

        insertInfo = QString("update '%1' set moduleType='%3',batch='%4',sn='%5',person='%6',checkTime='%7',moduleVersion='%8',LedResult='%9',beerReult='%10',pushButtonResult='%11',reedResult='%12',internelResult='%13',csqResult='%14',result='%15' where imei='%2';")
                .arg(createTableName(m_getTimeStamp(QDate::currentDate())))
                .arg(QString(list[0]))
                .arg(QString(list[1]))
                .arg(QString(list[2]))
                .arg(QString(list[3]))
                .arg(QString(list[4]))
                .arg(QString(list[5]))
                .arg(QString(list[6]))
                .arg(QString(list[7]))
                .arg(QString(list[8]))
                .arg(QString(list[9]))
                .arg(QString(list[10]))
                .arg(QString(list[11]))
                .arg(QString(list[12]))
                .arg(QString(list[13]));
    }
    return insertInfo;
}


void saveRecord::on_comboBox_batch_currentIndexChanged(const QString &arg1)
{
    qDebug()<<"arg1:"<<arg1;
    currentDataBase = arg1;

}


void saveRecord::on_comboBox_resultSelect_currentIndexChanged(int index)
{
    changeUserInfo->checkResult = index;
}

void saveRecord::on_pushButton_find_clicked()
{
    memccpy(&currentUserInfo,&changeUserInfo,'\0',sizeof(currentUserInfo));
//    // 查询
//     查询dataBasic
    if(changeUserInfo->deviceBatch != ui->comboBox_batch->currentText())
    {
        qDebug()<<"不一致,changeUserInfo->deviceBatch:"<<changeUserInfo->deviceBatch<<"ui->comboBox_batch->currentText():"<<ui->comboBox_batch->currentText();
        if(!openSqlDataBase(ui->comboBox_batch->currentText()))
        {
             qDebug()<<"打开失败";
        }
    }

    // 获取table
    ui->tableWidget->setRowCount(0);
    //获取检测记录中的选项
    QString totalReSult;
    if(0 != ui->comboBox_resultSelect->currentIndex())
    {
        totalReSult =  ui->comboBox_resultSelect->currentText();
    }
    for(int i = m_getTimeStamp(ui->dateEdit_startTime->date());i<=m_getTimeStamp(ui->dateEdit_endTime->date());i++)
    {
        // 获取table并添加到qtablewidget中
        tableToTableWidghtShow(1,totalReSult,false,i);
    }
    if(0 == ui->tableWidget->rowCount())
    {
        QMessageBox::information(this,"温馨提示",QDate::currentDate().toString()+"无记录，如需查询历史请选择相应日期或结果","确认");
    }


}



void saveRecord::on_dateEdit_startTime_editingFinished()
{
    currentUserInfo->checkStartTime = ui->dateEdit_startTime->dateTime().toTime_t();
    ui->dateEdit_endTime->setMinimumDate(ui->dateEdit_startTime->date());
}

void saveRecord::on_dateEdit_endTime_editingFinished()
{
    qDebug()<<"come here";
    if(ui->dateEdit_endTime->dateTime().toSecsSinceEpoch() < ui->dateEdit_startTime->dateTime().toSecsSinceEpoch())
    {
        QMessageBox::critical(this,"错误","终止日期不能小于起始日期","确认");
    }
    else
    {
        currentUserInfo->checkEndTime = ui->dateEdit_endTime->dateTime().toTime_t();
    }
}

void saveRecord::on_pushButton_exportDevImei_clicked()
{
    QString fileName;
    if(m_currentSavePath == NULL || m_currentSavePath.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(NULL,"选取文件",".","(*.xls *.xlsx)");
        if(fileName.isEmpty())
        {
            return;
        }
    }
    else
    {
        fileName = m_currentSavePath;
    }
    saveTabDataToFile(fileName,true);
}

void saveRecord::on_pushButton_exportList_clicked()
{
    QString fileName;
    if(m_currentSavePath == NULL || m_currentSavePath.isEmpty())
    {
        fileName = QFileDialog::getSaveFileName(NULL,"选取文件",".","(*.xls *.xlsx)");
        if(fileName.isEmpty())
        {
            return;
        }
    }
    else
    {
        fileName = m_currentSavePath;
    }
    saveTabDataListToFile(fileName,true);
}
