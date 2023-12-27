#ifndef DIALOGTOTALRESULT_H
#define DIALOGTOTALRESULT_H

#include <QDialog>

namespace Ui {
class DialogTotalResult;
}

class DialogTotalResult : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTotalResult(QString pushButtonText,QWidget *parent = nullptr);
    ~DialogTotalResult();

private slots:
    void on_pushButton_stop_clicked();

    void on_pushButton_opearte_clicked();

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

private:
    Ui::DialogTotalResult *ui;
    QPoint last_mouse_position;
};

#endif // DIALOGTOTALRESULT_H
