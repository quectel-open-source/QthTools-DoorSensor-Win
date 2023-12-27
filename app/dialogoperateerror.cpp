#include "dialogoperateerror.h"
#include "ui_dialogoperateerror.h"
#include "appConfig.h"
#include <QMouseEvent>
signalType errorSelectNum;

DialogOperateError::DialogOperateError(QString text,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogOperateError)
{
    ui->setupUi(this);
    // this->setWindowFlags( Qt::WindowTitleHint);
    this->setWindowModality(Qt::ApplicationModal);
    // this->setWindowFlags(Qt::CoverWindow); // Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
    this->setWindowFlags(Qt::FramelessWindowHint);

    ui->pushButton_1->setText("结束检测");
    ui->pushButton_2->setText("重试");
    ui->pushButton_3->setText("下一步");
    if(!text.isEmpty())
    {
        if(text.contains(".png"))
        {
            QImage *img = new QImage(text);
            ui->label_showMessage->setFixedSize(img->size());
            ui->label_showMessage->setStyleSheet(QString("border-image:url(%1);").arg(text));
        }
        else
        {

            ui->label_showMessage->setStyleSheet("font: 75 20pt \"微软雅黑\";");
            ui->label_showMessage->setText("\""+text+"信息\"获取"+"<font color=red>不通过</font>");
        }
    }
    ui->label_image->setStyleSheet("border-image: url(:/image/critical_icon.png);");
    ui->label_image->setFixedSize(30,30);
    errorSelectNum = SIGNAL_INIT;
}

DialogOperateError::~DialogOperateError()
{
    delete ui;
}

void DialogOperateError::setNoNext()
{
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_3->setStyleSheet("font: 75 12pt \"微软雅黑\";background-color: rgb(169,169,169);color: rgb(225, 225, 225);");
}

void DialogOperateError::on_pushButton_1_clicked()
{
//    emit signalSelect(SIGNAL_STOP_CHECK);
    errorSelectNum = SIGNAL_STOP_CHECK;
    this->reject();
}

void DialogOperateError::on_pushButton_2_clicked()
{
//    emit signalSelect(SIGNAL_RETRY);
    errorSelectNum = SIGNAL_RETRY;
    this->reject();
}

void DialogOperateError::on_pushButton_3_clicked()
{
//    emit signalSelect(SIGNAL_NEXT);
    errorSelectNum = SIGNAL_NEXT;
    this->reject();
}

void DialogOperateError::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        last_mouse_position = event->globalPos();
}

void DialogOperateError::mouseMoveEvent(QMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton))
            return;
    const QPoint position = pos() + event->globalPos() - last_mouse_position; //the position of mainfrmae + (current_mouse_position - last_mouse_position)
    move(position.x(), position.y());
    last_mouse_position = event->globalPos();
}
