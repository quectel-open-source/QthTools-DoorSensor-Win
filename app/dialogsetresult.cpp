#include "dialogsetresult.h"
#include "ui_dialogsetresult.h"
#include <QMouseEvent>
DialogSetResult::DialogSetResult(QString text,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetResult)
{
    ui->setupUi(this);
//    this->setWindowTitle("指令已发送，请确认下列检测项");
    ui->label_tittle->setText("指令已发送，请确认下列检测项");
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlags(Qt::FramelessWindowHint);
    ui->label->setStyleSheet("font: 75 14pt \"微软雅黑\";");
    ui->label->setText(text);
    ui->pushButton_ok->setStyleSheet("QPushButton{border-image:url(:/image/check_func_ok.png)}");
    ui->pushButton_error->setStyleSheet("QPushButton{border-image:url(:/image/check_func_failed.png)}");
    ui->pushButton_reset->setStyleSheet("QPushButton{border-image:url(:/image/check_func_reset_gray.png)}");
    ui->pushButton_reset->setEnabled(false);
}

DialogSetResult::~DialogSetResult()
{
    delete ui;
}

void DialogSetResult::on_pushButton_ok_clicked()
{
    ui->pushButton_error->hide();
    ui->pushButton_reset->setStyleSheet("QPushButton{border-image:url(:/image/check_func_reset.png)}");
    ui->pushButton_reset->setEnabled(true);
    result = STATUS_OK;
}

void DialogSetResult::on_pushButton_error_clicked()
{
    ui->pushButton_ok->hide();
    ui->pushButton_reset->setStyleSheet("QPushButton{border-image:url(:/image/check_func_reset.png)}");
    ui->pushButton_reset->setEnabled(true);
    result = STATUS_ERROR;
}


void DialogSetResult::on_pushButton_reset_clicked()
{
    if(ui->pushButton_error->isHidden())
    {
        ui->pushButton_error->show();
    }
    if(ui->pushButton_ok->isHidden())
    {
        ui->pushButton_ok->show();
    }
    ui->pushButton_reset->setStyleSheet("QPushButton{border-image:url(:/image/check_func_reset_gray.png)}");
    ui->pushButton_reset->setEnabled(false);
    result = STATUS_INIT;
}

void DialogSetResult::on_pushButton_stop_clicked()
{
    QStringList list;
    if(STATUS_INIT != result)
    {
        list<< QString::number(result == STATUS_OK ? 1 : 0);
    }
    emit signalResult(SIGNAL_STOP_CHECK,list);
}

void DialogSetResult::on_pushButton_retry_clicked()
{
    QStringList list;
    emit signalResult(SIGNAL_RETRY,list);
}

void DialogSetResult::on_pushButton_next_clicked()
{
    if(STATUS_INIT == result)
    {
        QMessageBox::critical(this,"错误","请点击当前正确或错误图标","确认");
        return;
    }
    QStringList list;
    if(STATUS_INIT != result)
    {
        list<< QString::number(result == STATUS_OK ? 1 : 0);
    }
    emit signalResult(SIGNAL_NEXT,list);
}

void DialogSetResult::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        last_mouse_position = event->globalPos();
}

void DialogSetResult::mouseMoveEvent(QMouseEvent *event)
{
    if (!event->buttons().testFlag(Qt::LeftButton))
            return;
    const QPoint position = pos() + event->globalPos() - last_mouse_position; //the position of mainfrmae + (current_mouse_position - last_mouse_position)
    move(position.x(), position.y());
    last_mouse_position = event->globalPos();
}
