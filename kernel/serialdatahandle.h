#ifndef SERIALDATAHANDLE_H
#define SERIALDATAHANDLE_H

#include <QByteArray>
#include <QVariant>

typedef enum
{
    CMD_PRODUCT_TEST_INIT = 0x0001,
    CMD_PRODUCT_TEST_INIT_ACK = 0x0002,
    CMD_PRODUCT_TEST_RESULT = 0x0003,
    CMD_PRODUCT_TEST_RESULT_ACK = 0x0004,
    CMD_SYSTEM_END,
    CMD_BUSINESS_START = 0x0FFF,
    CMD_MODULE_INFO_GET = 0x1000,
    CMD_MODULE_INFO_GET_ACK = 0x1001,
    CMD_LED_CHECK = 0x1002,
    CMD_LED_CHECK_ACK = 0x1003,
    CMD_BEER_CHECK = 0x1004,
    CMD_BEER_CHECK_ACK = 0x1005,
    CMD_PUSHBUTTON_CHECK = 0x1006,
    CMD_PUSHBUTTON_CHECK_ACK = 0x1007,
    CMD_REED_CHECK = 0x1008,
    CMD_REED_CHECK_ACK = 0x1009,
    CMD_INTERNET_CHECK = 0x100A,
    CMD_INTERNET_CHECK_ACK = 0x100B,
    CMD_BUSINESS_END,
}cmdProtool;


/* 协议坐标 */
#define DP_POS_VER_1 0
#define DP_POS_VER_2 1
#define DP_POS_LEN_1 2
#define DP_POS_LEN_2 3
#define DP_POS_SUM 4
#define DP_POS_PID_1 5
#define DP_POS_PID_2 6
#define DP_POS_CMD_1 7
#define DP_POS_CMD_2 8
#define DP_POS_DATA 9

/* 协议版本 */
#define DP_VER_HEADER 0xAA  /* 协议版本固定头部 */
#define DP_VER_CURRENT 0xAA /* 当前协议版本 */
#define DP_VER_ESC 0x55     /* 协议版本转义符号 */

class serialDataHandle
{
public:
    serialDataHandle();
    bool serialDataAnalysis(QByteArray data,QByteArray *decodeData);
    QByteArray sendBasicData(int pkId,int cmd);
    QByteArray sendExpandData(int pkId,int cmd,QByteArray payload);
    QList<int> decode(QList<int> data);
    QByteArray encode(QByteArray data);
private:
    bool analysisHeader(QList<int> data);
    bool analysisLen(QList<int> data,int *len);
    bool analysisDataSum(QList<int> data);
    bool analysisPacketId(QList<int> data);
    bool analysisDataCmd(QList<int> data);
    uint8_t getDataSum(QByteArray array);
};

#endif // SERIALDATAHANDLE_H
