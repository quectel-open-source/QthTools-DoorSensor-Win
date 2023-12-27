#ifndef DIALOGTIPSCOMBINEDEND_H
#define DIALOGTIPSCOMBINEDEND_H

#include <QDialog>

namespace Ui {
class DialogTipsCombinedEnd;
}

class DialogTipsCombinedEnd : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTipsCombinedEnd(QString text,QWidget *parent = nullptr);
    DialogTipsCombinedEnd(int mode=0);
    ~DialogTipsCombinedEnd();
    void showPushButton();

private slots:
    void on_pushButton_abandon_clicked();

    void on_pushButton_endError_clicked();

    void closeEvent(QCloseEvent *event);

private:
    Ui::DialogTipsCombinedEnd *ui;
};

#endif // DIALOGTIPSCOMBINEDEND_H
