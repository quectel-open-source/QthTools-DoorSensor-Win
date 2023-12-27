#ifndef MYTOOLBOX_H
#define MYTOOLBOX_H

#include <QWidget>
#include <QToolBox>

class myToolBox : public QToolBox
{
    Q_OBJECT
public:
    myToolBox(QWidget *parent = nullptr);
};

#endif // MYTOOLBOX_H
