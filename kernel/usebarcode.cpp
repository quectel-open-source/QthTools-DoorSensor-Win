#include "usebarcode.h"
#include "kernel/usebarcode.h"
#include <QPainter>
#include "third/barcode.h"
#include <QDebug>

useBarcode::useBarcode(QWidget *parent) : QWidget(parent)
{

}



void useBarcode::setBarcodeAttribute(QString data)
{
    BarCode code128;
    QString barCodes;
    if((barCodes=code128.process128BCode(data))!=NULL)
    {
        barcodeString = barCodes;
        barcodeStringLen=barCodes.size()*imageWidthPixel;
    }
}

void useBarcode::setImageHeight(int height)
{
    imageHeight = height;
}

void useBarcode::setImageWidthPixel(int pixel)
{
    imageWidthPixel = pixel;
}

void useBarcode::drawBarCode(QPainter &painter, int x, int y)
{
    qDebug()<<"barcodeString:"<<barcodeString;
    painter.setPen(QPen(Qt::black,imageWidthPixel));//设置绘图画笔 黑色 两个像素
    for(int i=0;i<barcodeString.size();i++)
    {
        if(barcodeString.at(i)=='b')//画两个像素宽的黑线,一个像素太窄，扫描不出来 针对code128
        {
            painter.drawLine(x+i*imageWidthPixel,y,x+i*imageWidthPixel,y+imageHeight);
        }
        else if(barcodeString.at(i)=='1')//针对EAN13 因为起始符 中间分隔符 终止符的条形线较长一些，需要单独处理
        {
            //因为EAN13的数字位数是固定13个 所以所占的条形模块数是固定的，起始符 中间分隔符 终止符位置也是固定的
            if((i>10&&i<14)||(i>55&&i<61)||(i>102&&i<106))
            {
                painter.drawLine(x+i*imageWidthPixel,y,x+i*imageWidthPixel,y+imageHeight+6);
            }
            else
            {
                painter.drawLine(x+i*imageWidthPixel,y,x+i*imageWidthPixel,y+imageHeight);
            }
        }
    }
}

QPixmap useBarcode::saveImage()
{
    if(0 == barcodeStringLen || barcodeString.isEmpty())
    {
        return QPixmap();
    }
    QPixmap image(barcodeStringLen+10,imageHeight+10);
    image.fill();
    //QPainter 默认只能在paintEvent中调用，且被绘图的设备也需要重写了paintEvent
    QPainter painter(&image);//在box上定义一个绘图对象
    painter.setPen(Qt::NoPen);//因为要使用drawRect()只填充颜色，不画边缘线，也就不需要画笔
    painter.setBrush(QBrush(Qt::white));//二维码背景区域为白色
    int posX = 5;//条形码左顶点在box中的x位置
    int posY = 5;//条形码左顶点在box中的y位置
    //先画一个比条形码略大的区域
    painter.drawRect(posX,posY,barcodeStringLen+10,imageHeight+10);
    //画条形码
    drawBarCode(painter,posX,posY);
    return image;
}

void useBarcode::showImage(QWidget *widget)
{
    if(0 == barcodeStringLen)
    {
        return;
    }
    //QPainter 默认只能在paintEvent中调用，且被绘图的设备也需要重写了paintEvent
    QPainter painter(widget);//在box上定义一个绘图对象
    painter.setPen(Qt::NoPen);//因为要使用drawRect()只填充颜色，不画边缘线，也就不需要画笔
    painter.setBrush(QBrush(Qt::white));//二维码背景区域为白色
    int posX = (widget->width()-barcodeStringLen)/2;//条形码左顶点在box中的x位置
    int posY = (widget->height()-imageHeight)/2;//条形码左顶点在box中的y位置
    //先画一个比条形码略大的区域
    painter.drawRect(posX-4,posY-12,barcodeStringLen+8,imageHeight+24);
    //画条形码
    drawBarCode(painter,posX,posY);
}

void useBarcode::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
//    QGroupBox::paintEvent(event);//先调用父类的重绘事件处理函数，不然默认的groupbox的边框和标题都不在显示
//    if(barCodeWidth==0)
//    {
//        return;
//    }
//    //QPainter 默认只能在paintEvent中调用，且被绘图的设备也需要重写了paintEvent
//    QPainter painter(this);//在box上定义一个绘图对象
//    painter.setPen(Qt::NoPen);//因为要使用drawRect()只填充颜色，不画边缘线，也就不需要画笔
//    painter.setBrush(QBrush(Qt::white));//二维码背景区域为白色
//    posX = (this->width()-barCodeWidth)/2;//条形码左顶点在box中的x位置
//    posY = (this->height()-BARCODE_HEIGHT)/2;//条形码左顶点在box中的y位置
//    //先画一个比条形码略大的区域
//    painter.drawRect(posX-4,posY-12,barCodeWidth+8,BARCODE_HEIGHT+24);
//    //画条形码
//    drawBarCode(painter,posX,posY);
}
