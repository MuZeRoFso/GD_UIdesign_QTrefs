#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    winsize=new QSize(1400,820);
    this->resize(*winsize);
    this->setObjectName("MainWindow");
    QFile stylesheetfile(":/qss/source/qss/MainWidget.css");
    if(stylesheetfile.open(QIODevice::ReadOnly)){
    QString MainWidgetStyleSheet=stylesheetfile.readAll();
    this->setStyleSheet(MainWidgetStyleSheet);
    stylesheetfile.close();
    }
    ico=new QIcon(":/Image/source/Image/icon.png");
    sysico=new QSystemTrayIcon(*ico,this);
    this->setWindowIcon(*ico);
    sysico->setToolTip("SICS");
    sysico->show();
    titlebar=new TitleBar(this);
    titlebar->setTitleBarWidth(*winsize);
    connect(titlebar,&TitleBar::signalButtonMinClicked,this,&MainWidget::onButtonMinClicked);
    connect(titlebar,&TitleBar::signalButtonExitClicked,this,&MainWidget::onButtonExitClicked);
    funcwidget=new FunctionalWidget(this);
    mainlayout=new QVBoxLayout(this);
    mainlayout->setSpacing(0);
    this->setLayout(mainlayout);
    mainlayout->addWidget(titlebar);
    mainlayout->addWidget(funcwidget);
}

void MainWidget::onButtonMinClicked(){ this->showMinimized(); }

void MainWidget::onButtonExitClicked(){ close();}

void MainWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget,&opt,&p,this);
}

MainWidget::~MainWidget()
{
    delete winsize;
    delete ico;
}
