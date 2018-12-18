#include "content.h"
#include <QApplication>
#include <QTextCodec>
#include <QSplitter>
#include <QListWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFont font("AR PL KaitiM GB",12);	//设置整个程序采用的字体与字号
    a.setFont(font);
    //新建一个水平分割窗对象，作为主布局框
    QSplitter *splitterMain =new QSplitter(Qt::Horizontal,0);
    //用于设定分割窗的分隔条在拖动时是否为实时更新显示,若设置为true则为实时更新显示
    splitterMain->setOpaqueResize(true);
    QListWidget *list =new QListWidget(splitterMain);	//新建列表选择框
    list->insertItem(0,QObject::tr("基本信息"));
    list->insertItem(1,QObject::tr("联系方式"));
    list->insertItem(2,QObject::tr("家庭地址"));
    //主窗口新建一个窗体，包含三部分
    Content *content =new Content(splitterMain);
    //设置主布局框的标题
    splitterMain->setWindowTitle(QObject::tr("用户资料"));
    //设置主布局框的最小尺寸
    splitterMain->setMinimumSize(splitterMain->minimumSize());
    //设置主布局框的最大尺寸
    splitterMain->setMaximumSize(splitterMain->maximumSize());
    //链接槽函数，列表选项改变跳转到对应右边堆栈窗函数
    QObject::connect(list,SIGNAL(currentRowChanged(int)),content->stack,
    SLOT(setCurrentIndex(int)));
    QObject::connect(content->CloseBtn,SIGNAL(clicked(bool)),qApp,SLOT(quit()));

    splitterMain->show();	//显示主布局框，其上面的控件一同显示

    return a.exec();
}
