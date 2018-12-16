#include "dialog.h"
#include <QGridLayout>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent)
{
    label1=new QLabel(this);
    label2=new QLabel(this);
    lineEdit=new QLineEdit(this);
    button=new QPushButton(this);

    label1->setText(tr("radus:"));
    button->setText(tr("area"));
    QGridLayout *mainLayout=new QGridLayout(this);
    mainLayout->addWidget(label1,0,0);
    mainLayout->addWidget(lineEdit,0,1);
    mainLayout->addWidget(button,1,0);
    mainLayout->addWidget(label2,1,1);

    connect(button,SIGNAL(clicked()),this,SLOT(showArea()));

}
void Dialog::showArea()
{
    bool ok;
    QString tempStr;
    QString valueStr=lineEdit->text();
    int valueInt=valueStr.toInt(&ok);
    double area=valueInt*valueInt*3.14159;
    label2->setText(tempStr.setNum(area));
}
Dialog::~Dialog()
{

}
