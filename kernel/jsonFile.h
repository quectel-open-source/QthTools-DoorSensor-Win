#ifndef JSONFILE_H
#define JSONFILE_H
#include <QString.h>
#include <QDebug.h>
#include <QJsonDocument.h>
#include <QJsonObject.h>
#include <QJsonArray.h>
#include <QJsonValue.h>

class jsonFile
{
public:
    jsonFile(QString fileUrl);
    ~jsonFile();
    bool writeFile(void);
    bool readFile(void);
    bool readJsonKeyValue(QString keyUrl,QJsonValue *value);
    bool writeJsonKeyValue(QString keyUrl,QJsonValue value);
    bool isJsonObject(QByteArray readData);
    bool deleteJsonKey(QString key);
    bool clearFile();
private:
    QJsonObject jsonObj;
    QString fileUrl;
};

#endif // JSONFILE_H
