#ifndef SAVERECORD_H
#define SAVERECORD_H

#include <QWidget>

namespace Ui {
class saveRecord;
}


class saveRecord_userInfo
{
public:
    saveRecord_userInfo(){}
    void init()
    {
        checkStartTime = 0;
        checkEndTime = 0;
        checkResult = 0;
    }
    QString deviceBatch;
    uint64_t checkStartTime;
    uint64_t checkEndTime;
    int checkResult;
};


class saveRecord : public QWidget
{
    Q_OBJECT

public:
    explicit saveRecord(QWidget *parent = nullptr);
    ~saveRecord();
    bool openSqlDataBase(QString batch);
    bool insertHistoryToSql(QStringList list);
    bool modifyHistoryToSql(QStringList list);
    bool isOpenTable();
    void showWidget(QRect rect);
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_comboBox_batch_currentIndexChanged(const QString &arg1);

    void on_comboBox_resultSelect_currentIndexChanged(int index);

    void on_pushButton_find_clicked();

    void on_dateEdit_startTime_editingFinished();

    void on_dateEdit_endTime_editingFinished();

    void on_pushButton_exportDevImei_clicked();

    void on_pushButton_exportList_clicked();

private:
    void uiAttributeSet(void);

    void initSqlDataBase();
    void tableToTableWidghtShow(int mode,QString totalResult,bool needTips=true,int selectDay=-1);
    void sqlHistoryShow(QString tableName,QString totalResult,int mode=0);
    void insertTableWidget(QStringList data,int insertRow);
    QString createTableName(int64_t days);
    void saveTabDataToFile(QString fileName,bool noNeedTip);
    void saveTabDataListToFile(QString fileName,bool noNeedTip);
    bool matchFileHeader(QStringList list,int mode=0);
    QString packageSqlString(QStringList list,int mode=0);
private:
    Ui::saveRecord *ui;
    saveRecord_userInfo *currentUserInfo = NULL;
    saveRecord_userInfo *changeUserInfo= NULL;
    QObject *recordOfBatchSql = NULL;
    QObject *recordOfManageSql = NULL;
    QString currentDataBase;
    QString m_currentSavePath;
};

#endif // SAVERECORD_H
