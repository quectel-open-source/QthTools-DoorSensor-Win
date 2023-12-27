#ifndef USERCONFIG_H
#define USERCONFIG_H
#include <QString>
#include <QDebug>

#define APP_NAME "QthTools-DoorSensor-Win"
#define APP_VER "1.0.0"
extern QString vitalFilePath;

class userConfig
{
public:
    userConfig()
    {
        deviceReady = false;
        csqMin = 0 ;
        csqMax = 31;
        isFirstShow = true;
        isNeedOperate = false;
        isCheckingInit = false;
        isCheckingEnd = false;
    }
public:
    QString moduleType;     // 模组类型
    QString batch;          // 当前批次号
    QString personName;     // 当前检测人
    bool deviceReady;     // 设备是否正常开启
    int csqMin;           // 信号最小值
    int csqMax;           // 信号最大值
    bool isFirstShow;         // 是否为首次
    bool isNeedOperate;     // 单元测试中
    bool isCheckingInit;
    bool isCheckingEnd;
};

#endif // APPCONFIG_H
