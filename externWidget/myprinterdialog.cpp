#include "myprinterdialog.h"
#include "ui_myprinterdialog.h"
#include <qDebug>
myPrinterDialog::myPrinterDialog(QStringList info,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::myPrinterDialog)
{
    ui->setupUi(this);
    basicInfo = info;
    if(2 == basicInfo.count())
    {
        if(0 != QString(basicInfo[0]).toInt())
        {
            ui->spinBox_label_copies->setValue(QString(basicInfo[0]).toInt());
        }
        if(0 == QString(basicInfo[1]).toInt() || 1 == QString(basicInfo[1]).toInt())
        {
            ui->comboBox->setCurrentIndex(QString(basicInfo[1]).toInt());
        }
    }
}

myPrinterDialog::~myPrinterDialog()
{
    delete ui;
}


void myPrinterDialog::on_pushButton_clicked()
{

    basicInfo[0] = QString::number(ui->spinBox_label_copies->value());
    basicInfo[1] = QString::number(ui->comboBox->currentIndex());
    emit sendPrinterBasicInfo(basicInfo);
    this->reject();
}

void myPrinterDialog::on_pushButton_2_clicked()
{
    this->reject();
}
