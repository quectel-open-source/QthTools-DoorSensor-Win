#ifndef MYPRINTERDIALOG_H
#define MYPRINTERDIALOG_H

#include <QDialog>

namespace Ui {
class myPrinterDialog;
}

class myPrinterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit myPrinterDialog(QStringList info,QWidget *parent = nullptr);
    ~myPrinterDialog();
signals:
    void sendPrinterBasicInfo(QStringList);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::myPrinterDialog *ui;
    QStringList basicInfo;
};

#endif // MYPRINTERDIALOG_H
