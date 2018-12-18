#ifndef CONTENT_H
#define CONTENT_H

#include <QDialog>
#include <QStackedWidget>
#include <QPushButton>
#include "baseinfo.h"
#include "contact.h"
#include "homeaddr.h"
class Content : public QFrame
{
    Q_OBJECT

public:
    Content(QWidget *parent = 0);
    QStackedWidget *stack;
    QPushButton *AmendBtn;
    QPushButton *CloseBtn;
    BaseInfo  *baseInfo;
    Contact *contact;
    Homeaddr *homeaddr;
    ~Content();
};

#endif // CONTENT_H
