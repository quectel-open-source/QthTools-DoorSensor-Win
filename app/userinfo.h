#ifndef USERINFO_H
#define USERINFO_H

#include <QWidget>
#include <QCompleter>
#include <QStandardItemModel>
#include <QStringListModel>
#include "app/appinfosave.h"
namespace Ui {
class userInfo;
}



class userInfo : public QWidget
{
    Q_OBJECT

public:
    explicit userInfo(QWidget *parent = nullptr);
    ~userInfo();

signals:
    void signalUserInfo(QStringList);

private slots:
    void on_lineEdit_batchSelect_editingFinished();

    void on_lineEdit_personSelect_editingFinished();

    void on_comboBox_moduleSelect_currentIndexChanged(int index);

    void on_pushButton_clicked();


private:
    void uiAttributeSet(void);
//    bool eventFilter(QObject *widget, QEvent *event);
private:
    Ui::userInfo *ui;
    QStringListModel *batchListModel;
    QStringListModel *personListModel;
    appInfoSave *gc_appInfoSaveInstance = NULL;
};

#endif // USERINFO_H
