#include "mainwindow.h"
#include <QApplication>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include <QTextCodec>
#include "userConfig.h"
#include "mainattribute.h"

QString vitalFilePath = QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation)+"/QthTools-DoorSensor-Win/"; 

int main(int argc, char *argv[])
{
    if(!initAppDirPath())
    {
        return 0;
    }
    initDebugType();
    setScreenDimensions();
    QApplication a(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    QSystemSemaphore semaphore("SingleAppTestSemaphore", 1);
    semaphore.acquire();

#ifndef Q_OS_WIN32
    // 在linux / unix 程序异常结束共享内存不会回收
    // 在这里需要提供释放内存的接口，就是在程序运行的时候如果有这段内存 先清除掉
    QSharedMemory nix_fix_shared_memory("SingleAppTestSemaphore");
    if (nix_fix_shared_memory.attach())
    {undefined
        nix_fix_shared_memory.detach();
    }
#endif
    QSharedMemory sharedMemory(APP_NAME);
    bool isRunning = false;
    if (sharedMemory.attach())
    {
        isRunning = true;
    }
    else
    {
        sharedMemory.create(1);
        isRunning = false;
    }
    semaphore.release();

    // 如果您已经运行了应用程序的一个实例，那么我们将通知用户。
    if (isRunning)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText("The application is already running.\n"
            "Allowed to run only one instance of the application.");
        msgBox.exec();
        return 1;
    }


#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("GBK");
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#endif

   setbuf(stdout, NULL);//让printf立即输出

    MainWindow w;
    return a.exec();
}
