#include "content.h"

Content::Content(QWidget *parent)
    : QFrame(parent)
{
    stack =new QStackedWidget(this);	//创建一个QStackedWiget对象
    //对堆栈窗口的显示风格进行设置
    stack->setFrameStyle(QFrame::Panel|QFrame::Raised);
    /* 插入三个页面 */
    baseInfo =new BaseInfo();
    contact =new Contact();
    homeaddr =new Homeaddr();
    stack->addWidget(baseInfo);
    stack->addWidget(contact);
    stack->addWidget(homeaddr);
    /* 创建两个按钮 */
    AmendBtn =new QPushButton(tr("修改"));
    CloseBtn =new QPushButton(tr("关闭"));
    QHBoxLayout *BtnLayout =new QHBoxLayout;
    BtnLayout->addStretch(1);//靠右固定
    BtnLayout->addWidget(AmendBtn);
    BtnLayout->addWidget(CloseBtn);
    /* 进行整体布局 */
    QVBoxLayout *RightLayout =new QVBoxLayout(this);
    RightLayout->setMargin(10);
    RightLayout->setSpacing(6);
    RightLayout->addWidget(stack);
    RightLayout->addLayout(BtnLayout);
}

Content::~Content()
{

}
