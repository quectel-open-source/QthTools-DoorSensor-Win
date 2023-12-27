#include "userinfo.h"
#include "ui_userinfo.h"
#include <qDebug>
#include <QMessageBox>

userInfo::userInfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userInfo)
{
    ui->setupUi(this);
    this->setWindowTitle("移远门磁产测");
    this->setWindowIcon(QIcon(":/image/quectel.ico"));
    this->setWindowFlags(this->windowFlags()& ~Qt::WindowMinMaxButtonsHint);
    this->setStyleSheet("#userInfo{background-color:rgb(255,255,255)}");
    uiAttributeSet();

}

userInfo::~userInfo()
{
    delete gc_appInfoSaveInstance;
    delete ui;
}

/**************************************************************************
** 功能	@brief : 控件属性设置
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void userInfo::uiAttributeSet()
{

    batchListModel = new QStringListModel(this);
    personListModel = new QStringListModel(this);

    gc_appInfoSaveInstance = new appInfoSave("app_login_userInfo");
    int valueType = 0;
    QVariant value;
    if(gc_appInfoSaveInstance->findAppValue("app_batchList",&valueType,&value))
    {
        batchListModel->setStringList(value.toStringList());
    }
    if(gc_appInfoSaveInstance->findAppValue("app_personList",&valueType,&value))
    {
        personListModel->setStringList(value.toStringList());
    }
    if(gc_appInfoSaveInstance->findAppValue("app_moduleSelect",&valueType,&value))
    {
        ui->comboBox_moduleSelect->setCurrentIndex(value.toInt());
    }

    QCompleter *batchCompleter = new QCompleter(this);
    batchCompleter->setMaxVisibleItems(9);
    batchCompleter->setModel(batchListModel);
    ui->lineEdit_batchSelect->setCompleter(batchCompleter);
    if(batchListModel->stringList().count() > 0)
    {
        ui->lineEdit_batchSelect->setText(batchListModel->stringList().at(0));
    }

    QCompleter *presonCompleter = new QCompleter(this);
    presonCompleter->setMaxVisibleItems(9);
    presonCompleter->setModel(personListModel);
    ui->lineEdit_personSelect->setCompleter(presonCompleter);   
    if(personListModel->stringList().count() > 0)
    {
        ui->lineEdit_personSelect->setText(personListModel->stringList().at(0));
    }

    QString text = ui->label_batch->text();
    text.append("<font color=red>*</font>");
    ui->label_batch->setText(text);

    text = ui->label_module->text();
    text.append("<font color=red>*</font>");
    ui->label_module->setText(text);

    text = ui->label_person->text();
    text.append("<font color=red>*</font>");
    ui->label_person->setText(text);

    ui->lineEdit_batchSelect->setMaxLength(32);
    ui->lineEdit_personSelect->setMaxLength(16);

}

/**************************************************************************
** 功能	@brief : 批次号输入框输入完成回调
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void userInfo::on_lineEdit_batchSelect_editingFinished()
{
    QString text = ui->lineEdit_batchSelect->text();
    if(!text.isEmpty())
    {
        QStringList list =  batchListModel->stringList();
        if(!list.contains(text, Qt::CaseSensitive))
        {
            if(batchListModel->rowCount() > 8)
            {
                batchListModel->removeRow(batchListModel->rowCount()-1);
            }
            batchListModel->insertRow(0);
            QModelIndex index=batchListModel->index(0,0);
            batchListModel->setData(index,text,Qt::DisplayRole);//设置显示文字
            gc_appInfoSaveInstance->modifyAppInfo("app_batchList",TYPE_STRINGLIST,batchListModel->stringList());
        }
    }
}
/**************************************************************************
** 功能	@brief : 检测人输入框输入完成回调
** 输入	@param :
** 输出	@retval:
***************************************************************************/
void userInfo::on_lineEdit_personSelect_editingFinished()
{
    QString text = ui->lineEdit_personSelect->text();
    if(!text.isEmpty())
    {
        QStringList list =  personListModel->stringList();
        if(!list.contains(text, Qt::CaseSensitive))
        {
            if(personListModel->rowCount() > 8)
            {
                personListModel->removeRow(personListModel->rowCount()-1);
            }
            personListModel->insertRow(0);
            QModelIndex index=personListModel->index(0,0);
            personListModel->setData(index,text,Qt::DisplayRole);//设置显示文字
            gc_appInfoSaveInstance->modifyAppInfo("app_personList",TYPE_STRINGLIST,personListModel->stringList());
        }
    }
}

void userInfo::on_comboBox_moduleSelect_currentIndexChanged(int index)
{
    gc_appInfoSaveInstance->modifyAppInfo("app_moduleSelect",TYPE_INT,index);
}

void userInfo::on_pushButton_clicked()
{
    if(ui->lineEdit_batchSelect->text().isEmpty() || ui->lineEdit_personSelect->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请先输入批次号或检测人","确认");
        return;
    }
    QString moduleName = ui->comboBox_moduleSelect->currentText();
//    if(0 == ui->comboBox_moduleSelect->currentIndex())
//    {
//        moduleName = "BC25";
//    }
//    else if(1 == ui->comboBox_moduleSelect->currentIndex())
//    {
//        moduleName = "BY26";
//    }
    QStringList  list;
    list<<moduleName<<ui->lineEdit_batchSelect->text()<<ui->lineEdit_personSelect->text();
    emit signalUserInfo(list);
}

//bool userInfo::eventFilter(QObject *widget, QEvent *event)
//{
//	if (widget->objectName() == "lineEdit_batchSelect")
//	{
//		if (event->type() == QEvent::FocusIn)
//		{

//		}
//	}
//	return QWidget::eventFilter(widget, event);
//}
