#ifndef DIALOGOPERATEERROR_H
#define DIALOGOPERATEERROR_H

#include <QDialog>
#include "app/appConfig.h"
namespace Ui {
class DialogOperateError;
}


class DialogOperateError : public QDialog
{
    Q_OBJECT

public:
    explicit DialogOperateError(QString text,QWidget *parent = nullptr);
    ~DialogOperateError();
    void setNoNext();
signals:
    void signalSelect(int);
private slots:
    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::DialogOperateError *ui;
    QPoint last_mouse_position;
};

#endif // DIALOGOPERATEERROR_H
