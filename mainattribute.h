#ifndef MAINATTRIBUTE_H
#define MAINATTRIBUTE_H
#include <QtGlobal>
#include "third/ccrashstack.h"

class mainAttribute
{
public:
    mainAttribute();
};


void printfLogToFile(QtMsgType type,const QMessageLogContext &context,const QString &msg);
long __stdcall   callback(_EXCEPTION_POINTERS*   excp);

bool initAppDirPath();
void initDebugType(bool mode=true);
bool checkAppIsUnique();
void setScreenDimensions();

#endif // MAINATTRIBUTE_H
