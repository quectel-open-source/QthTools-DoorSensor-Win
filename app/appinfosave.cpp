#include "app/appinfosave.h"
#include "appConfig.h"
#include "userConfig.h"
appInfoSave::appInfoSave(QString fileUrl)
{
    appInfoFileName = vitalFilePath+fileUrl;
    this->appInfoFile = new jsonFile(appInfoFileName);
    this->appInfoFile->readFile();
}

bool appInfoSave::addAppInfo(QString key, int valueType, QVariant value)
{
    bool result = true;
    switch(valueType)
    {
        case TYPE_BOOL:
            appInfoFile->writeJsonKeyValue(key,value.toBool());
            break;
        case TYPE_INT:
            appInfoFile->writeJsonKeyValue(key,value.toInt());
            break;
        case TYPE_DOUBLE:
            appInfoFile->writeJsonKeyValue(key,value.toDouble());
            break;
        case TYPE_STRING:
            appInfoFile->writeJsonKeyValue(key,value.toString());
            break;
        case TYPE_STRINGLIST:
        {
            QStringList list = value.toStringList();
            QJsonArray array;
            for(int i=0;i<list.size();i++)
            {
                array.append(QJsonValue(QString(list[i])));
            }
            appInfoFile->writeJsonKeyValue(key,array);
            break;
        }
        case TYPE_JOSN_OBJECT:
        {
            appInfoFile->writeJsonKeyValue(key,value.toJsonObject());
            break;
        }
        default:
            result = false;
            break;
    }
    if(result)
    {
        appInfoFile->writeFile();
    }
    return result;
}

bool appInfoSave::deleteAppInfo(QString key, int valueType, QVariant value)
{
    Q_UNUSED(valueType);
    Q_UNUSED(value);
    return appInfoFile->deleteJsonKey(key);
}

bool appInfoSave::findAppValue(QString key, int *valueType, QVariant *value)
{
    QJsonValue jsonValue;
    if(appInfoFile->readJsonKeyValue(key,&jsonValue))
    {
        if(jsonValue.isBool())
        {
            *valueType = TYPE_BOOL;
            *value = QVariant(jsonValue.toBool());
        }
        else if(jsonValue.isDouble())
        {
            double number = jsonValue.toDouble();
            if((number-int(number)) > 0)
            {
                *valueType = TYPE_DOUBLE;
                *value = QVariant(number);
            }
            else
            {
                *valueType = TYPE_INT;
                *value = QVariant(jsonValue.toInt());
            }
        }
        else if(jsonValue.isString())
        {
            *valueType = TYPE_STRING;
            *value = QVariant(jsonValue.toString());
        }
        else if(jsonValue.isArray())
        {
            QJsonArray array = jsonValue.toArray();
            QStringList list;
            for(int i = 0;i<array.size();i++)
            {
                if(!array[i].isString())
                {
                    return false;
                }
                else
                {
                    list.append(array[i].toString());
                }
            }
            *valueType = TYPE_STRINGLIST;
            *value = QVariant(list);
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool appInfoSave::findAppKey(QStringList *key)
{
    Q_UNUSED(key);
    return true;
}

bool appInfoSave::modifyAppInfo(QString key, int valueType, QVariant value)
{
    addAppInfo(key,valueType,value);
    return true;
}

void appInfoSave::clearFile()
{
    appInfoFile->clearFile();
}







