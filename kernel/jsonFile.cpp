#include "jsonFile.h"
#include "QFileInfo"
#include "QFile"
#include "QDir"

jsonFile::jsonFile(QString fileUrl)
{
    qInfo()<<"jsonfileUrl:"<<fileUrl;
    this->fileUrl = fileUrl;
    QFileInfo fileInfo(fileUrl);
    QString path = fileInfo.path();
    QDir dir;
    if(path.isEmpty() || dir.exists(path)==false) dir.mkdir(path);
    if(!fileInfo.isFile())
    {
        writeFile();
    }
}

jsonFile::~jsonFile()
{

}

/**************************************************************************
** 功能	@brief : 读取配置文件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::readFile(void)
{
    qInfo()<<__FUNCTION__;
    QJsonObject obj;
    QFile file(this->fileUrl);
    if(true == file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray readData = file.readAll();
        file.close();
        return isJsonObject(readData);
    }
    qCritical()<<"read <"<<file.fileName()<<"> fail";
    return false;
}


bool jsonFile::isJsonObject(QByteArray readData)
{
    QJsonParseError jsonErr;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(readData,&jsonErr);
    if(jsonErr.error == QJsonParseError::NoError)
    {
        if(jsonDoc.isObject())
        {
            this->jsonObj = jsonDoc.object();
            return true;
        }
    }
    return false;
}

bool jsonFile::deleteJsonKey(QString key)
{
    QStringList keyList = key.split("/",QString::SplitBehavior::SkipEmptyParts);
    QJsonObject obj = this->jsonObj;
    for(int i=0;i<keyList.size();i++)
    {
        if(i+1 == keyList.size())
        {
            if(obj.contains(keyList.at(i)))
            {
                obj.remove(keyList.at(i));
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(obj.contains(keyList.at(i)) && obj.value(keyList.at(i)).isObject())
        {
            obj = obj.value(keyList.at(i)).toObject();
        }
        else
        {
            break;
        }
    }
    return false;
}

bool jsonFile::clearFile()
{
    QFile file(fileUrl);
    bool ret = file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();
    return ret;
}

/**************************************************************************
** 功能	@brief : 写配置文件
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::writeFile(void)
{
    QFile file(fileUrl);
    if(true == file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
    {
        QJsonDocument rootDoc;
        rootDoc.setObject(this->jsonObj);
        QByteArray writeData = rootDoc.toJson();
        file.write(writeData);
        file.close();
        return true;
    }
    qCritical()<<"write <"<<file.fileName()<<"> fail";
    return false;
}


/**************************************************************************
** 功能	@brief : 读json值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::readJsonKeyValue(QString keyUrl,QJsonValue *value)
{
//    qInfo()<<__FUNCTION__;
    QStringList keyList = keyUrl.split("/",QString::SplitBehavior::SkipEmptyParts);
    QJsonObject obj = this->jsonObj;
    for(int i=0;i<keyList.size();i++)
    {
        if(i+1 == keyList.size())
        {
            if(obj.contains(keyList.at(i)))
            {
                *value = obj.value(keyList.at(i));
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(obj.contains(keyList.at(i)) && obj.value(keyList.at(i)).isObject())
        {
            obj = obj.value(keyList.at(i)).toObject();
        }
        else
        {
            break;
        }
    }
    qDebug()<<"find no value";
    return false;
}
/**************************************************************************
** 功能	@brief : 写json值
** 输入	@param :
** 输出	@retval:
***************************************************************************/
bool jsonFile::writeJsonKeyValue(QString keyUrl,QJsonValue value)
{
//    qInfo()<<__FUNCTION__;
    QStringList keyList = keyUrl.split("/",QString::SplitBehavior::SkipEmptyParts);
    QList<QJsonObject> objList;
    QJsonObject obj = this->jsonObj;
    for(int32_t i=0;i<keyList.size()-1;i++)
    {
        if(obj.contains(keyList.at(i)))
        {
            obj = obj.value(keyList.at(i)).toObject();
        }
        else
        {
            QJsonObject tempObj;
            obj =tempObj;
        }
        objList.append(obj);
    }

    for(int i = objList.size();i>0;i--)
    {
        objList.last().insert(keyList.at(i),value);
        value = objList.last();
        objList.removeLast();
    }
    this->jsonObj.insert(keyList.first(),value);
    return true;
}




