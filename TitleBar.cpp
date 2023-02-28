#include "TitleBar.h"

TitleBar::TitleBar(QWidget *parent) : QWidget(parent)
{
    baselayout=new QHBoxLayout(this);
    QFile stylesheetfile(":/qss/source/qss/MainTitleBar.css");
    if(stylesheetfile.open(QIODevice::ReadOnly)){
        QString stylesheet=stylesheetfile.readAll();
        this->setStyleSheet(stylesheet);
    }
    init();
    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    this->setFixedHeight(TITLE_BAR_HEIGHT);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setObjectName("MainTitleBar");
    builtConnections();
}

void TitleBar::init()
{
    titleicon=new QPixmap(":/Image/source/Image/icon.png");
    minicon=new QPixmap(":/Image/source/Image/hide.png");
    exiticon=new QPixmap(":/Image/source/Image/exit.png");
    icon=new QLabel(this);
    icon->setPixmap(titleicon->scaled(30,30));
    l_icon=new QLabel("SICSDB",this);
    l_icon->setAlignment(Qt::AlignCenter);
    l_icon->setFixedHeight(BUTTON_HEIGHT);
    title=new QLabel("涉密信息检查系统",this);
    title->setAlignment(Qt::AlignCenter);
    title->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    b_min=new QPushButton(this);
    b_exit=new QPushButton(this);
    title->setFixedHeight(BUTTON_HEIGHT);
    b_min->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);
    b_min->setIcon(minicon->scaled(30,30));
    b_min->setToolTip("收起");
    b_exit->setFixedSize(BUTTON_WIDTH,BUTTON_HEIGHT);
    b_exit->setIcon(exiticon->scaled(30,30));
    b_exit->setToolTip("关闭");
    baselayout->addWidget(icon);
    baselayout->addWidget(l_icon);
    baselayout->addWidget(title);
    baselayout->addWidget(b_min);
    baselayout->addWidget(b_exit);
}

void TitleBar::setTitleBarWidth(const QSize &size){this->setFixedWidth(size.width());}

void TitleBar::builtConnections()
{   //绑定信号与槽
    connect(b_min,&QPushButton::clicked,this,&TitleBar::onButtonMinClicked);
    connect(b_exit,&QPushButton::clicked,this,&TitleBar::onButtonExitClicked);
}

void TitleBar::onButtonMinClicked(){emit signalButtonMinClicked();}

void TitleBar::onButtonExitClicked(){emit signalButtonExitClicked();}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    QWidget *myparent=static_cast<QWidget*>(this->parent());
    diffX=event->globalPosition().x()-myparent->x();
    diffY=event->globalPosition().y()-myparent->y();
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    QWidget *myparent=static_cast<QWidget*>(this->parent());
    myparent->move(event->globalPosition().x()-diffX,event->globalPosition().y()-diffY);
}

void TitleBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

TitleBar::~TitleBar()
{
    delete titleicon;
    delete minicon;
    delete exiticon;
}
