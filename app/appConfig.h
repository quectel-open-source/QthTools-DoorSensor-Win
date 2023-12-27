#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QMessageBox>
#include <QDebug>

typedef enum
{
    RESULT_CHECK_INIT =-1,
    RESULT_MODULE_INFO_LINE = 0,
    RESULT_LED_LINE,
    RESULT_BEER_LINE,
    RESULT_PUSHBUTTON_LINE,
    RESULT_REED_LINE,
    RESULT_INTERNET_LINE,
    RESULT_CSQ_LINE,
    RESULT_CHECK_END = 99,
}resultWidgetChange;

#define RESULT_MESS_STRING(X) ((X == RESULT_MODULE_INFO_LINE) ? "模组" : (X == RESULT_LED_LINE)     ? "指示灯"       \
                                                : (X == RESULT_BEER_LINE)   ? "蜂鸣器" \
                                                : (X == RESULT_PUSHBUTTON_LINE)    ? "按键"  \
                                                : (X == RESULT_REED_LINE)      ? "干簧管"    \
                                                : (X == RESULT_INTERNET_LINE)     ? "联网"   \
                                                : (X == RESULT_CSQ_LINE)   ? "CSQ" \
                                                : "Unknown")

typedef enum
{
    SIGNAL_INIT =0,
    SIGNAL_STOP_CHECK,
    SIGNAL_RETRY,
    SIGNAL_NEXT,
    SIGNAL_OK,
    SIGNAL_FAILED,
}signalType;

typedef enum
{
    DEV_FUNC_STATUS_INIT = 0,
    DEV_FUNC_STATUS_ERROR,
    DEV_FUNC_STATUS_OK,
}deviceResultStatus;

#define RESULT_STATUS_STRING(X)  (                                                                                                   \
    (X == DEV_FUNC_STATUS_INIT) ? "-" : (X == DEV_FUNC_STATUS_ERROR)     ? "NG"       \
                             : (X == DEV_FUNC_STATUS_OK)     ? "OK"       \
                             : "Unknown")

class checkStatusInfo
{
public:
    checkStatusInfo()
    {
        init();
    }
    void init()
    {
        ledResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
        beerResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
        pushButtonResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
        reedResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
        internelResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
        csq = 0;
        csqResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
        totalResult = RESULT_STATUS_STRING(DEV_FUNC_STATUS_INIT);
        IMEI.clear();
        sn.clear();
        version.clear();
        pk.clear();
    }
    QString getTotalResult()
    {
        if(RESULT_STATUS_STRING(DEV_FUNC_STATUS_OK) != ledResult || RESULT_STATUS_STRING(DEV_FUNC_STATUS_OK) != beerResult || RESULT_STATUS_STRING(DEV_FUNC_STATUS_OK) != pushButtonResult || RESULT_STATUS_STRING(DEV_FUNC_STATUS_OK) != reedResult \
             || RESULT_STATUS_STRING(DEV_FUNC_STATUS_OK) != internelResult || RESULT_STATUS_STRING(DEV_FUNC_STATUS_OK) != csqResult  )
        {
            return "不通过";
        }
        return "通过";
    }
public:
    QString IMEI;           // 模组IMEI
    QString sn;             // 模组sn
    QString version;        // 模组版本
    QString pk;
    QString ledResult;         // 指示灯检测结果
    QString beerResult;        // 蜂鸣器检测结果
    QString pushButtonResult;   // 按键检测结果
    QString reedResult;        // 干簧管检测结果
    QString internelResult;    // 联网检测结果
    int csq;                // 信号值
    QString csqResult;         // 信号值检测结果
    QString totalResult;       // 整体检测结果
};


typedef struct
{
    int currentEventLine;
    bool checkDeviceResult;
    bool serialStatus;
    bool printerStatus;
}userDataStruct;


extern userDataStruct m_userInfo;
extern signalType errorSelectNum;


#endif // APPCONFIG_H
