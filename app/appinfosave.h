#ifndef APPINFOSAVE_H
#define APPINFOSAVE_H

#include <QMap>
#include <QVariant>
#include "kernel/jsonFile.h"

typedef enum
{
    TYPE_BOOL= 0,
    TYPE_INT ,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_STRINGLIST,
    TYPE_JOSN_OBJECT,
}supportType;

class appInfoSave
{
public:
    appInfoSave(QString fileUrl=nullptr);
    bool addAppInfo(QString key,int valueType,QVariant value);
    bool deleteAppInfo(QString key,int valueType,QVariant value);
    bool findAppValue(QString key,int *valueType,QVariant *value);
    bool findAppKey(QStringList *key);
    bool modifyAppInfo(QString key,int valueType,QVariant value);
    void clearFile();
private:
    bool initFile(QString fileUrl);

private:
    QMap<QVariant,QVariant>appInfo;
    jsonFile *appInfoFile;
    QString appInfoFileName;

};

#endif // APPINFOSAVE_H
