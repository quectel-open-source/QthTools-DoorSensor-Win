#ifndef TOOLUPDATE_H
#define TOOLUPDATE_H

#include <QWidget>
#include "third/QSimpleUpdater/src/Updater.h"
#include "third/QSimpleUpdater/src/Downloader.h"
#include "third/QSimpleUpdater/include/QSimpleUpdater.h"


namespace Ui {
class toolUpdate;
}

class toolUpdate : public QWidget
{
    Q_OBJECT

public:
    explicit toolUpdate(QWidget *parent = nullptr);
    ~toolUpdate();
public slots:
    void checkForUpdates();
signals:
//    void stopSubThread();
private slots:
    void resetFields();
    void updateChangelog(const QString &url);
    void displayAppcast(const QString &url, const QByteArray &reply);
    void updataExeEvent(QString newSoftName);
private:
    Ui::toolUpdate *ui;
//    QNetworkAccessManager *manager;
    QSimpleUpdater *m_updater;
    bool firstUse = false;
};

#endif // TOOLUPDATE_H
