#ifndef USEPRINTER_H
#define USEPRINTER_H
#include <QString>
#include <QPrinter>
#include <QPixmap>



class usePrinterInfoClass
{
public:
    usePrinterInfoClass()
    {
        copies = 1;
        pagOrder = QPrinter::FirstPageFirst;
        pageSize = QPrinter::A4;
        orientation = QPrinter::Portrait;
        paperSource = QPrinter::Auto;
        left = 0.0;
        top = 0.0;
        right = 0.0;
        bottom = 0.0;
        unit = 0;
        colorMode = 0;
    }

public:
    QString name;       // 打印机名字
    int copies;         // 份数
    QPrinter::PageOrder pagOrder;       // 打印顺序
    QPrinter::PageSize pageSize;       // 纸张大小
    QPrinter::Orientation orientation;    // 打印顺序 横纵
    QPrinter::PaperSource paperSource;    // 纸张来源

//    int pagOrder;       // 打印顺序
//    int pageSize;       // 纸张大小
//    int orientation;    // 打印顺序 横纵
//    int paperSource;    // 纸张来源

    double left;        // 页边距
    double top;
    double right;
    double bottom;
    int unit;           // 使用的单位
    int colorMode;
};

class usePrinter: public QObject
{
    Q_OBJECT
public:
    explicit usePrinter(QObject *parent = nullptr);
    ~usePrinter();
    bool initPrinter(QString printerName);
//    void startPrinter(QPixmap &pixmap,int mode = -1);
//    void startPrinterPage();
//    void startPrintPreview(QPixmap &pixmap);
//    void setPrintPixmap(QPixmap &pixmap);
//    QPixmap getPrintPixmap();
//    void PrintPreview();
////    void printQString(QString data);
//    void printImage(QString imagePath);

    void initPrinter(usePrinterInfoClass info);
    void setPrinterAttribute();
    void setPrinterPageAttribute();
    void printPixmap(QPixmap &pixmap);
    QString getCurrentPrinterName();
    usePrinterInfoClass getPrinterInfo();
private slots:
    void recvPrinterBasicInfo(QStringList basicInfo);
//    void Slot_Preview(QPrinter* printer);
private:
    QPrinter *m_pPrinter = NULL;
//    QPixmap m_Pixmap;
    usePrinterInfoClass gc_printerInfo;
};

#endif // USEPRINTER_H
