#include "dialogtipscombinedend.h"
#include "ui_dialogtipscombinedend.h"
#include "appConfig.h"
#include <QCloseEvent>

DialogTipsCombinedEnd::DialogTipsCombinedEnd(QString text, QWidget *parent):
    QDialog(parent),
ui(new Ui::DialogTipsCombinedEnd)
{
    ui->setupUi(this);
    this->setWindowTitle("检测界面");
    this->setWindowIcon(QIcon(":/image/quectel.ico"));
    this->setWindowFlags(Qt::WindowCloseButtonHint);
//    Qt::CustomizeWindowHint
    ui->label->setText(text);

    ui->pushButton_abandon->hide();
    ui->pushButton_endError->hide();
    errorSelectNum = SIGNAL_INIT;
}

DialogTipsCombinedEnd::DialogTipsCombinedEnd(int mode)
{
//     emit signalResult(mode);
    Q_UNUSED(mode);
}

DialogTipsCombinedEnd::~DialogTipsCombinedEnd()
{
    delete ui;
}

void DialogTipsCombinedEnd::showPushButton()
{
    if(ui->pushButton_abandon->isHidden())
    {
        ui->pushButton_abandon->show();
    }
    if(ui->pushButton_endError->isHidden())
    {
        ui->pushButton_endError->show();
    }
}

void DialogTipsCombinedEnd::on_pushButton_abandon_clicked()
{
    errorSelectNum = SIGNAL_STOP_CHECK;
    reject();
}

void DialogTipsCombinedEnd::on_pushButton_endError_clicked()
{
    errorSelectNum = SIGNAL_FAILED;
    reject();
}

void DialogTipsCombinedEnd::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
}
