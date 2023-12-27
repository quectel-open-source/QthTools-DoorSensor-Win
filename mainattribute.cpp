#include "mainattribute.h"
#include <QDir>
#include <QMutex>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QStandardPaths>
#include <QDebug>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include <QCoreApplication>
#include "userConfig.h"


mainAttribute::mainAttribute()
{

}

QFile *file = nullptr;

//设置日志输出方式

void printfLogToFile(QtMsgType type,const QMessageLogContext &context,const QString &msg)
{
    Q_UNUSED(type);
    Q_UNUSED(context);
    static QMutex mutex;
    mutex.lock();
    QString current_date_time = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss zzz");
    QString message = QString("[%1] %2 %3").arg(current_date_time).arg(msg).arg("\r\n");
    QTextStream text_stream(file);
    text_stream << message;
    file->flush();
    mutex.unlock();
}

long __stdcall   callback(_EXCEPTION_POINTERS*   excp)
{
    CCrashStack crashStack(excp);
    QString sCrashInfo = crashStack.GetExceptionInfo();
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyyMMddhhmmsszzz");
    QString sFileName =QString("%1/dump_").arg(vitalFilePath)+current_date+".log";

    QFile file(sFileName);
    if (file.open(QIODevice::WriteOnly|QIODevice::Truncate))
    {
        file.write(sCrashInfo.toUtf8());
        file.close();
    }
    return   EXCEPTION_EXECUTE_HANDLER;
}

bool initAppDirPath()
{
    QDir dir(vitalFilePath);
    if(!dir.exists())
    {
        if(!dir.mkdir(vitalFilePath))
        {
            QMessageBox::critical(NULL,"Dump",QString::fromLocal8Bit("<FONT size=4><div><b>程序无法使用</b><br/></div>"),QString::fromLocal8Bit("确认"));
            return false;
        }
    }
    return true;
}

void initDebugType(bool mode)
{
    Q_UNUSED(mode);
  if(mode)
  {

      QDateTime current_date_time =QDateTime::currentDateTime();
      QString current_date =current_date_time.toString("yyyyMMddhhmmsszzz");
      QString logFile =  QString("%1/log_").arg(vitalFilePath)+current_date+".txt";
      file = new QFile(logFile);
      if(file->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Truncate))
      {
          qCritical()<<"create or open log file failed";
      }
      qInstallMessageHandler(printfLogToFile);

      SetUnhandledExceptionFilter(callback);
  }
}

bool checkAppIsUnique()
{
    QSystemSemaphore semaphore("SingleAppTestSemaphore", 1);
    semaphore.acquire();

#ifndef Q_OS_WIN32
    // 在linux / unix 程序异常结束共享内存不会回收
    // 在这里需要提供释放内存的接口，就是在程序运行的时候如果有这段内存 先清除掉
    QSharedMemory nix_fix_shared_memory("SingleAppTest2");
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
        return false;
    }
    return true;
}

void setScreenDimensions()
{
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    #endif
}
