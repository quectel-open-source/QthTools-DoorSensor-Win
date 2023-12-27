#include "dialogtotalresult.h"
#include "ui_dialogtotalresult.h"
#include "appConfig.h"
#include <QMouseEvent>

DialogTotalResult::DialogTotalResult(QString pushButtonText,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTotalResult)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->pushButton_opearte->setText(pushButtonText);
    if(0 == pushButtonText.compare("通过"))
    {
        ui->pushButton_opearte->setStyleSheet("QPushButton{background-color: rgb(0, 133, 189);font: 75 12pt \"微软雅黑\";color: rgb(255, 255, 255);}");
    }
    else
    {
        ui->pushButton_opearte->setStyleSheet("QPushButton{background-color: rgb(255, 0, 0);font: 75 12pt \"微软雅黑\";color: rgb(255, 255, 255);}");

    }
}

DialogTotalResult::~DialogTotalResult()
{
    delete ui;
}

void DialogTotalResult::on_pushButton_stop_clicked()
{
    errorSelectNum = SIGNAL_STOP_CHECK;
    reject();
}

void DialogTotalResult::on_pushButton_opearte_clicked()
{
    if(0 == ui->pushButton_opearte->text().compare("通过"))
    {
        errorSelectNum = SIGNAL_OK;
    }
    else
    {
        errorSelectNum = SIGNAL_FAILED;
    }
    reject();
}

void DialogTotalResult::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        last_mouse_position = event->globalPos();
}

void DialogTotalResult::mouseMoveEvent(QMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton))
            return;
    const QPoint position = pos() + event->globalPos() - last_mouse_position; //the position of mainfrmae + (current_mouse_position - last_mouse_position)
    move(position.x(), position.y());
    last_mouse_position = event->globalPos();
}
