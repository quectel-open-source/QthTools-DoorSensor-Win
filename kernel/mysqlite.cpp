#include "mysqlite.h"
#include <QStandardPaths>
#include <QDebug>
#include<QDir>
mySqlite::mySqlite()
{

}

mySqlite::~mySqlite()
{

}

void mySqlite::createSqlDataBase(QString name,QString path)
{
    // 指定的连接名称
    QString dstPath = path;
    if (QSqlDatabase::contains(name))
    {
        database = QSqlDatabase::database(name);
    }
    else
    {
        qDebug()<<"dstPath:"<<dstPath;
        // 添加数据库
        database = QSqlDatabase::addDatabase("QSQLITE",name);
        // 数据库文件名:如果这个数据库不存在，则会在后续操作时自动创建；如果已经存在，则后续的操作会在已有的数据库上进行
        database.setDatabaseName("./"+dstPath+".db");
//        database.setUserName("root");  //设置数据库登录用户名
//        database.setPassword("123456");//设置数据库登录密

    }
}
/**************************************************************************
** 功能	@brief : 更新sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note: 存在表则切换，不存在则创建
**      @example: create table student (id int primary key, name varchar(30), age int)
***************************************************************************/
void mySqlite::createSqlQuery(QString tableName, QString data)
{
    // 打开数据库
    if(!database.open())
    {
        qCritical()<<"open error";
        return;
    }
    bool isFind = findTableName(tableName);
    if(false == isFind)
    {
        query = new QSqlQuery(database);
        if(false == query->prepare(data))
        {
            qDebug()<<"error table"<<query->lastError();
        }
        if(query->exec(data))
        {
            qDebug()<<"query->lastError():"<<query->lastError();
        }
    }
    else
    {
        query = new QSqlQuery(database);
        if(query->exec(data))
        {
            qDebug()<<"query->lastError():"<<query->lastError();
        }
    }
    m_currentTableName = tableName;
}
/**************************************************************************
** 功能	@brief : 插入sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: insert into student value(3, \"Li\", 23)
***************************************************************************/
bool mySqlite::insertSqlData(QString data,int *errorColumn)
{
    if(!database.isOpen())
    {
        return false;
    }
    if(!query->exec(data))
    {
        qDebug() <<"insert error:" <<query->lastError();
        if(query->lastError().text().contains("imei"))
        {
            *errorColumn = 2;
        }
        return false;
    }
    return true;
}

int mySqlite::batchInsertSqlData(QList<structInfo_t> data)
{
    if(!database.isOpen())
    {
        return -1;
    }
    int sum = data.size();
    if(sum > 0)
    {
        database.transaction();
//        QStringList dkList;
//        QStringList pkList;
//        QStringList snList;
//        QStringList textList;

//        query->prepare("insert into device_userInfo(dk,pk,sn,text) values(?,?,?,?);");
//        for(int i=0;i<sum;i++)
//        {
//            dkList.append(data[i].dk);
//            pkList.append(data[i].pk);
//            snList.append(data[i].sn);
//            textList.append(data[i].text);
//        }
//        query->addBindValue(dkList);
//        query->addBindValue(pkList);
//        query->addBindValue(snList);
//        query->addBindValue(textList);
//        query->execBatch();
        for(int i=0;i<sum;i++)
        {
            if(!query->exec(QString("insert into device_userInfo(dk,pk,sn,text) values('%1','%2','%3','%4')").arg(data[i].dk).arg(data[i].pk).arg(data[i].sn).arg(data[i].text)))
            {
                qDebug() <<"insert error:" <<query->lastError();
                database.rollback();
                return i;
            }
        }
        database.commit();
    }
    return 0;
}


/**************************************************************************
** 功能	@brief : 查找sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: select id, name from student"; select * from student select max(id) from student
***************************************************************************/
QStringList mySqlite::getSqlDataList(QString data)
{
    qDebug()<<__FUNCTION__;
    QStringList result;
    if(!database.isOpen())
    {
        return result;
    }
    if(!query->exec(data))
    {
        qDebug() << query->lastError();
        return result;
    }
    QSqlRecord rec = query->record();
    int cols = rec.count();
    while(query->next())
    {
        QString info;
        for(int i =0;i<cols;i++)
        {
            info.append(query->value(i).toString()+",");
        }
        info.chop(1);
        result.append(info);
    }
    return result;
}


QStringList mySqlite::execSqlDataList(QString data)
{

    QStringList result;
    if(!database.isOpen())
    {
        goto exit;
    }
    if(!query->exec(data))
    {
        qDebug() << query->lastError();
        goto exit;
    }
    while(query->next())
    {
        result.append(query->value(0).toString());
    }
exit:
    return result;
}



QString mySqlite::getSqlDataString(QString data)
{
    QString result;
    if(!database.isOpen())
    {
        goto exit;
    }
        // 打开数据库
    if(!query->exec(data))
    {
        qDebug() << query->lastError();
        goto exit;
    }
    while(query->next())
    {
        result.append(query->value(0).toString());
    }
exit:
    return result;
}

bool mySqlite::findSqlData(QString data)
{
    bool result = false;
    if(database.isOpen() && query->exec(data))
    {
        if(query->next())
        {
            qDebug()<<"size:"<<query->size();
            if(query->size() > 0)
                result = true;
        }

    }
    return result;
}

QVector<structInfo_t> mySqlite::findSqlAllList(QString data)
{
    QVector<structInfo_t> it;
    if(!database.isOpen())
    {
        goto exit;
    }
    // 打开数据库
    if(!query->exec(data))
    {
        qDebug() << query->lastError();
        return it;
    }
    while(query->next())
    {
        structInfo_t info;
        info.dk = query->value(1).toString();
        info.pk = query->value(2).toString();
        info.sn = query->value(3).toString();
        info.text = query->value(4).toString();
        info.ver = query->value(5).toString();
        it.append(info);
    }
exit:
    return it;
}


/**************************************************************************
** 功能	@brief : 删除sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: delete from student where id = ?"; delete from student  delete from student where id=1 and name='xiaopi'
***************************************************************************/
bool mySqlite::delSqlData(QString data)
{
    if(!database.isOpen())
    {
        return false;
    }
    // 打开数据库
    if(!query->exec(data))
    {
        qDebug() << query->lastError();
        return false;
    }
    return true;
}



int mySqlite::batchDelSqlData(QList<QString> data)
{
    if(!database.isOpen())
    {
        return -1;
    }
    int sum = data.size();
    if(sum > 0)
    {
        database.transaction();
        for(int i=0;i<sum;i++)
        {
            if(!query->exec(QString("delete from device_userInfo where dk='%1';").arg(QString(data[i]))))
            {
                qDebug() <<"insert error:" <<query->lastError();
                database.rollback();
                return i;
            }
        }
        database.commit();
    }
    return 0;
}


/**************************************************************************
** 功能	@brief : 更新sqltable数据
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example: update student set name='小皮' where id=1
***************************************************************************/
bool mySqlite::modifySqlData(QString data)
{
#ifdef ERROR_LEVEL
    qDebug()<<__FUNCTION__;
#endif
    // 打开数据库
    if(!database.isOpen())
    {
        return false;
    }
    if(!query->exec(data))
    {
        qDebug() << query->lastError();
        return false;
    }

    return true;
}

/**************************************************************************
** 功能	@brief : 关闭sql数据库
** 输入	@param :
** 输出	@retval:
** 注意  @note:
**      @example:
***************************************************************************/
void mySqlite::closeSqlDataBase()
{
    // 打开数据库
    if(!database.isOpen())
    {
        qCritical()<<"open error";
        return;
    }
    database.close();
}

QString mySqlite::getTableName()
{
    return m_currentTableName;
}

bool mySqlite::findTableName(QString findName)
{
    QStringList tableNameList = database.tables();
    bool isFind = false;
    qDebug()<<tableNameList;
    foreach (QString name, tableNameList)
    {
        // sqlite_sequence 排除
        if ( QString("sqlite_sequence") == name)
        {
            continue;
        }
        if(0 == findName.compare(name))
        {
            isFind = true;
            break;
        }
    }
    return isFind;
}
