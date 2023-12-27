#include "serialdatahandle.h"
#include <QDebug>

serialDataHandle::serialDataHandle()
{

}

bool serialDataHandle::serialDataAnalysis(QByteArray data,QByteArray *decodeData)
{
    int dataLen = 0;
    QList<int> numData;
    if(data.size()%2 != 0)
    {
        return false;
    }
    for(int i=0;i<data.size();i+=2)
    {
        numData.append(data.mid(i,2).toUInt(nullptr,16));
    }

    if(analysisHeader(numData.mid(0,2)))
    {
        QList<int> decodePayload = decode(numData.mid(2));
        if(analysisLen(decodePayload,&dataLen) && analysisDataSum(decodePayload.mid(2)) \
                && analysisPacketId(decodePayload.mid(3,2)) && analysisDataCmd(decodePayload.mid(5,2)))
        {
            for(int i=0;i<numData.size();i++)
            {
                (*decodeData).append(numData.at(i));
            }
            return true;
        }
    }
    qCritical()<<"data analysis failed";
    return false;
}

/* 接收处理 */


QByteArray serialDataHandle::sendBasicData(int pkId,int cmd)
{
    QByteArray info;
    info.append(char(0xAA));
    info.append(char(0xAA));

    QByteArray payload;
    payload.append(char(0x00));
    payload.append(char(0x05));
    payload.append((((pkId >> 8) & 0xFF)+ (pkId & 0xFF)+ ((cmd >> 8) & 0xFF) +(cmd & 0xFF)) & 0xFF);

    payload.append((pkId >> 8) & 0xFF);
    payload.append(pkId & 0xFF);
    payload.append((cmd >> 8) & 0xFF);
    payload.append(cmd & 0xFF);

    info.append(encode(payload.toHex()));

    return info;
}

QByteArray serialDataHandle::sendExpandData(int pkId,int cmd,QByteArray payload)
{
    QByteArray info;
    info.append(char(0xAA));
    info.append(char(0xAA));

    QByteArray byteArray;
    byteArray.append((pkId >> 8) & 0xFF);
    byteArray.append(pkId & 0xFF);
    byteArray.append((cmd >> 8) & 0xFF);
    byteArray.append(cmd & 0xFF);
    byteArray.append(payload);


    int insideHeaderLen = byteArray.count() + 1;

    QByteArray newByteArray;
    newByteArray[0] = (insideHeaderLen >> 8)& 0xFF;
    newByteArray[1] = insideHeaderLen & 0xFF;
    newByteArray[2] = getDataSum(byteArray);
    newByteArray.append(byteArray);
    info.append(encode(newByteArray.toHex()));

    return info;
}


bool serialDataHandle::analysisHeader(QList<int> data)
{
    qDebug()<<__FUNCTION__<<data[0]<<data[1];
    if(0xAA != data.at(0) || 0xAA != data.at(1))
    {
        return false;
    }
    return true;
}

bool serialDataHandle::analysisLen(QList<int> data,int *len)
{
    qDebug()<<__FUNCTION__;
    int actualLength = data.size() - 2;
    int protoolLength = ((data[0]<<8)&0xFF00) + (data[1] & 0xFF);
    qDebug()<<"actualLength:"<<actualLength<<"protoolLength:"<<protoolLength;
    if(actualLength != protoolLength || 0 == actualLength || 0 == protoolLength)
    {
        return false;
    }
    *len = protoolLength;
    return true;
}

bool serialDataHandle::analysisDataSum(QList<int> data)
{
    qDebug()<<__FUNCTION__;
    int protoolSum = data[0]& 0xFF;
    uint8_t actualSum = 0;
    for(int i = 0;i < data.size()-1;i++)
    {
        actualSum += data[1+i];
    }
    qDebug()<<"protoolSum:"<<protoolSum<<"actualSum:"<<actualSum;
    if(protoolSum != actualSum)
    {
        return false;
    }
    return true;
}

bool serialDataHandle::analysisPacketId(QList<int> data)
{
    qDebug()<<__FUNCTION__;
    int protoolPkId = ((data[0]<<8)&0xFF00) +(data[1]& 0xFF);

    if(protoolPkId > 5)
    {
        return false;
    }
    return true;
}

bool serialDataHandle::analysisDataCmd(QList<int> data)
{
    qDebug()<<__FUNCTION__;
    int protoolCmd = ((data[0]<<8)&0xFF00) +(data[1]& 0xFF);
    if((protoolCmd < CMD_SYSTEM_END )||(protoolCmd >CMD_BUSINESS_START && protoolCmd < CMD_BUSINESS_END))
    {
        return true;
    }
    return false;
}

uint8_t serialDataHandle::getDataSum(QByteArray array)
{
    uint8_t sum = 0;
    for(int i = 0;i< array.size();i++)
    {
        sum += array[i];
    }
    return sum;
}

QList<int> serialDataHandle::decode(QList<int> data)
{
    qDebug()<<__FUNCTION__;
    QList<int> newData;

    int i = 0;
    for(i=0;i<data.size()-3;i++)
    {
        qDebug()<<"data.at(i):"<<data.at(i);
        qDebug()<<"DP_VER_HEADER:"<<DP_VER_HEADER;
        if((DP_VER_HEADER == data.at(i) && DP_VER_ESC == data.at(i+1) && DP_VER_HEADER == data.at(i+2)) ||(DP_VER_HEADER == data.at(i) && DP_VER_ESC == data.at(i+1) && DP_VER_ESC == data.at(i+2) && DP_VER_HEADER == data.at(i+3)))
        {
            newData.append(data.at(i));
            i++;
        }
        else
        {
            newData.append(data.at(i));
        }
    }
    if(DP_VER_HEADER == data.at(i) && DP_VER_ESC == data.at(i+1) && DP_VER_HEADER == data.at(i+2))
    {
        newData.append(DP_VER_HEADER);
        newData.append(DP_VER_HEADER);
    }
    else
    {
        newData.append(data.at(i));
        newData.append(data.at(i+1));
        newData.append(data.at(i+2));
    }
    qDebug()<<"newData:"<<newData;
    return newData;
}

QByteArray serialDataHandle::encode(QByteArray data)
{
    QList<int> numData;
    for(int i=0;i<data.size();i+=2)
    {
        numData.append(data.mid(i,2).toUInt(nullptr,16));
    }
    QByteArray newData;
    int i = 0;
    for(i=0;i<numData.size()-1;i++)
    {
        if(DP_VER_HEADER == numData[i] && (DP_VER_CURRENT == numData[i+1] || DP_VER_ESC == numData[i+1]))
        {
            newData.append(numData.at(i));
            newData.append(DP_VER_ESC);
        }
        else
        {
            newData.append(numData.at(i));
        }
    }
    newData.append(numData.at(i));
    return newData;
}



/* 发送 */



