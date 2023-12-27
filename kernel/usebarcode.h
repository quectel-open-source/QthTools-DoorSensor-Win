#ifndef USEBARCODE_H
#define USEBARCODE_H

#include <QWidget>

class useBarcode : public QWidget
{
    Q_OBJECT
public:
    explicit useBarcode(QWidget *parent = nullptr);
    void setBarcodeAttribute(QString data);
    void setImageHeight(int height);
    void setImageWidthPixel(int pixel);
    QPixmap saveImage();
    void showImage(QWidget *widget);
signals:

public slots:

protected:
    void paintEvent(QPaintEvent * event);//重写绘图事件处理函数
private:
    void drawBarCode(QPainter &painter, int x, int y);
private:
    int imageHeight = 70;
    int imageWidthPixel = 2;
    QString barcodeString;
    int barcodeStringLen;
};

#endif // USEBARCODE_H
