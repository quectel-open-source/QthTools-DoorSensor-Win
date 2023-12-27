#ifndef MYSQLITE_H
#define MYSQLITE_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QtSql/QSqlDriver>    //数据库驱动种类相关头文件
#include <QtSql/QSqlRecord>    //数据库执行结果相关头文件
#include <QtSql/QSqlError>

typedef struct
{
    QString pk;
    QString dk;
    QString sn;
    QString text;
    QString ver;
}structInfo_t;

class mySqlite: public QObject
{
    Q_OBJECT
public:
    mySqlite();
    ~mySqlite();
    void createSqlDataBase(QString name,QString path);
    void createSqlQuery(QString tableName,QString data);
    bool insertSqlData(QString data,int *errorColumn);
    int batchInsertSqlData(QList<structInfo_t>data);
    QStringList getSqlDataList(QString data);
    QString getSqlDataString(QString data);
    bool findSqlData(QString data);
    QVector<structInfo_t> findSqlAllList(QString data);
    bool delSqlData(QString data);
    int batchDelSqlData(QList<QString> data);
    bool modifySqlData(QString data);
    QStringList execSqlDataList(QString data);
    void closeSqlDataBase();
    QString getTableName();
    bool findTableName(QString findName);

private:
    QSqlDatabase database;
    QSqlQuery *query;
    QString m_currentTableName;
};

#endif // MYSQLITE_H
