#include "useprinter.h"
#include <QPixmap>
#include <QPainter>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QPrintPreviewDialog>
#include<QDebug>
#include "externWidget/myprinterdialog.h"
#include <QMetaEnum>
#include <QPrinter>
// 后续此部分需要调整   2022/5/6

// 信息保存 和 打印设置，如何根据控件内容对应打印设计大小调整

usePrinter::usePrinter(QObject *parent) : QObject(parent)
{
    if(m_pPrinter == NULL)
        m_pPrinter = new QPrinter(QPrinter::PrinterResolution);
}

usePrinter::~usePrinter()
{
    if (m_pPrinter != nullptr)
        delete m_pPrinter;
    m_pPrinter = NULL;
}

bool usePrinter::initPrinter(QString printerName)
{
    if(m_pPrinter == NULL)
    {
        m_pPrinter = new QPrinter(QPrinter::PrinterResolution); // QPrinter::ScreenResolution
    }
    qDebug()<<"printerName:"<<printerName;
    m_pPrinter->setPrinterName(printerName);
    gc_printerInfo.name = printerName;
    return m_pPrinter->isValid();

}

void usePrinter::initPrinter(usePrinterInfoClass info)
{
    if(m_pPrinter == NULL)
    {
        m_pPrinter = new QPrinter(QPrinter::PrinterResolution); // QPrinter::ScreenResolution
    }
    gc_printerInfo = info;
    // 初始化
    m_pPrinter->setPrinterName(gc_printerInfo.name);
    m_pPrinter->setCopyCount(gc_printerInfo.copies);
    m_pPrinter->setPageOrder(gc_printerInfo.pagOrder);
    m_pPrinter->setPageSize(gc_printerInfo.pageSize);
    m_pPrinter->setOrientation(gc_printerInfo.orientation);
    m_pPrinter->setPaperSource(gc_printerInfo.paperSource);
    m_pPrinter->setPageMargins(gc_printerInfo.left,gc_printerInfo.top,gc_printerInfo.right,gc_printerInfo.bottom,(gc_printerInfo.unit == 0 ? QPrinter::Millimeter : \
                                                                                                            gc_printerInfo.unit == 1 ? QPrinter::Point : gc_printerInfo.unit == 2 ? QPrinter::Inch :\
                                                                                                            gc_printerInfo.unit == 3 ? QPrinter::Pica : gc_printerInfo.unit == 4 ? QPrinter::Didot :\
                                                                                                            gc_printerInfo.unit == 5 ? QPrinter::Cicero : QPrinter::DevicePixel ));
    m_pPrinter->setColorMode(gc_printerInfo.colorMode == 0 ? QPrinter::GrayScale : QPrinter::Color);

}


//void usePrinter::printImage(QString imagePath)
//{
//    if(!imagePath.isEmpty() && !gc_printerInfo.name.isEmpty())
//    {
//        QPixmap pixmap(imagePath);
//        QPrinter printer;
//        printer.setPrinterName(gc_printerInfo.name); //打印机名称
//        QPainter painter(&printer);
//        painter.drawText(150, 150, imagePath);
//        painter.drawPixmap(50, 100, pixmap);
//    }
//}

void usePrinter::setPrinterAttribute()
{
    if(!gc_printerInfo.name.isEmpty())
    {
        QStringList list;
        list.append(QString::number(gc_printerInfo.copies));
        list.append(QString::number(QPrinter::GrayScale == gc_printerInfo.colorMode? 0:1));
        myPrinterDialog *dialog = new myPrinterDialog(list);
        connect(dialog,SIGNAL(sendPrinterBasicInfo(QStringList)),this,SLOT(recvPrinterBasicInfo(QStringList)));
        dialog->exec();
    }
}

void usePrinter::setPrinterPageAttribute()
{
    if(!gc_printerInfo.name.isEmpty())
    {
        QPageSetupDialog pageSetUpdlg(m_pPrinter);
        if (pageSetUpdlg.exec() == QDialog::Accepted)
        {
            m_pPrinter->setOrientation(QPrinter::Landscape);
        }
        else
        {
            m_pPrinter->setOrientation(QPrinter::Portrait);
        }
        gc_printerInfo.pagOrder = m_pPrinter->pageOrder();
        gc_printerInfo.pageSize = m_pPrinter->pageSize();
        gc_printerInfo.orientation = m_pPrinter->orientation();
        gc_printerInfo.paperSource = m_pPrinter->paperSource();
        gc_printerInfo.top = m_pPrinter->margins().top;
        gc_printerInfo.bottom = m_pPrinter->margins().bottom;
        gc_printerInfo.left = m_pPrinter->margins().left;
        gc_printerInfo.right = m_pPrinter->margins().right;
        if(0 == m_pPrinter->margins().top && 0 == m_pPrinter->margins().bottom && 0 == m_pPrinter->margins().left && 0 == m_pPrinter->margins().right)
        {
            m_pPrinter->setFullPage(true);
            m_pPrinter->setPageMargins(0,0,0,0, QPrinter::Millimeter);
        }
    }
}

void usePrinter::printPixmap(QPixmap &pixmap)
{
    QPainter painterPixmap;
    painterPixmap.begin(m_pPrinter);
    painterPixmap.drawPixmap(0, 0, pixmap);
    painterPixmap.end();
}

QString usePrinter::getCurrentPrinterName()
{
    return gc_printerInfo.name;
}

usePrinterInfoClass usePrinter::getPrinterInfo()
{
    return gc_printerInfo;
}

void usePrinter::recvPrinterBasicInfo(QStringList basicInfo)
{
    if(2 == basicInfo.count())
    {
        m_pPrinter->setCopyCount(QString(basicInfo[0]).toInt());
        m_pPrinter->setColorMode(QString(basicInfo[1]).toInt() == 0 ? QPrinter::GrayScale : QPrinter::Color);
        gc_printerInfo.copies = m_pPrinter->copyCount();
        gc_printerInfo.colorMode = QString(basicInfo[1]).toInt();
        qDebug()<<"gc_printerInfo.copies:"<<gc_printerInfo.copies;
        qDebug()<<"gc_printerInfo.colorMode:"<<gc_printerInfo.colorMode;
    }
}


//void usePrinter::startPrinter(QPixmap &pixmap, int mode)
//{
//    if(!gc_printerName.isEmpty())
//    {
//        QPrintDialog printDialog(m_pPrinter);
//        if(-1 == mode)
//        {
//            if (printDialog.exec() == QDialog::Accepted)
//            {
//                QPainter painterPixmap;
//                painterPixmap.begin(m_pPrinter);
//                painterPixmap.drawPixmap(0, 0, pixmap);
//                painterPixmap.end();
//            }
//        }
//        else
//        {
//            QPainter painterPixmap;
//            painterPixmap.begin(m_pPrinter);
//            painterPixmap.drawPixmap(0, 0, pixmap);
//            painterPixmap.end();
//        }
//    }
//}

//void usePrinter::startPrinterPage()
//{
//    QPrinter printer;
//    QPageSetupDialog pageSetUpdlg(&printer);
//    if (pageSetUpdlg.exec() == QDialog::Accepted)
//    {
//        printer.setOrientation(QPrinter::Landscape);
//    }
//    else
//    {
//        printer.setOrientation(QPrinter::Portrait);
//    }

//}

//void usePrinter::startPrintPreview(QPixmap &pixmap)
//{
//    if (pixmap.isNull())
//        return;
//    setPrintPixmap(pixmap);
//    PrintPreview();
//}

//void usePrinter::setPrintPixmap(QPixmap &pixmap)
//{
//    m_Pixmap = pixmap;
//}

//QPixmap usePrinter::getPrintPixmap()
//{
//    return m_Pixmap;
//}

//void usePrinter::PrintPreview()
//{
//    QPrinter printer;
//    QPrintPreviewDialog previewDialog(&printer);
//    connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(Slot_Preview(QPrinter*)));
//    previewDialog.exec(); //打印预览对话框显示，paintRequest触发
//}

//void usePrinter::Slot_Preview(QPrinter *printer)
//{
//    QPainter painterPixmap;
//    painterPixmap.begin(printer);
//    //QRect rect = painterPixmap.viewport();
//    //int x = rect.width() / getPrintPixmap().width();
//    //int y = rect.height() / getPrintPixmap().height();
//    //painterPixmap.scale(x, y);
//    painterPixmap.drawPixmap(0, 0, getPrintPixmap());
//    painterPixmap.end();
//}

