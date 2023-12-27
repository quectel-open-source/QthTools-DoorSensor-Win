#ifndef DIALOGSETRESULT_H
#define DIALOGSETRESULT_H

#include <QDialog>
#include "app/appConfig.h"

typedef enum
{
    STATUS_INIT = 0,
    STATUS_OK,
    STATUS_ERROR,
}resultStatusStruct;


namespace Ui {
class DialogSetResult;
}


class DialogSetResult : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetResult(QString text,QWidget *parent = nullptr);
    ~DialogSetResult();
signals:
    void signalResult(int,QStringList);
private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_reset_clicked();

    void on_pushButton_error_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_retry_clicked();

    void on_pushButton_next_clicked();

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::DialogSetResult *ui;
    int result = STATUS_INIT;
    QPoint last_mouse_position;
};

#endif // DIALOGSETRESULT_H
